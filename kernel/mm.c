#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <raampjes/mm.h>
#include <raampjes/panic.h>
#include <raampjes/cpu.h>
#include <raampjes/interrupts.h>
#include <raampjes/sched.h>

uint32_t PageDirectory[1024] __attribute__((aligned(4096)));
uint32_t PageTable1[1024] __attribute__((aligned(4096)));

unsigned int dir_index(uintptr_t virtual_address);
unsigned int page_table_index(uintptr_t virtual_address);
uintptr_t get_free_address(uintptr_t target);
uintptr_t add_page_table(uintptr_t virtual_address, uint32_t flags);
uint32_t *get_page_entry(uintptr_t virtual_address);
void page_fault_handler(void);
uintptr_t get_page_table(uintptr_t virtual_address);
bool page_present(uintptr_t virtual_address);
void page_fault(Registers regs, uint32_t error_code);
void enable_CoW(uint32_t *table);
bool CoW_enabled(uintptr_t addr);
void pf_non_present(uint32_t addr);
void pf_rsvd();
void pf_ro_violation(uint32_t addr);


/* 
 * Initialize the memory manager.
 */
void init_mm(struct multiboot_tag_mmap *map) {
	init_phys_mm(map);
	/*
	 * We map the page directory to the last entry of itself, this way
	 * it maps all the present page tables to the last 1024 pages in the
	 * virtual address space.
	 */
	PageDirectory[1023] = ((uintptr_t)&PageDirectory - KERNEL_MEMORY) | 3;

	/* Because were now fully operating in the higher half we don't
	 * need the first 4MiB to be mapped, so we free it. */
	PageDirectory[0] &= ~1;

	current->page_directory = PageDirectory;
	set_interrupt_callback(0xE, &page_fault);
}

/*
 * Allocate a new page frame, put it at the 
 * first free virtual address, starting from the 'target' address and
 * return the virtual address.
 */
uintptr_t alloc_page(uintptr_t target, int flags) {
	uintptr_t page_frame = alloc_page_frame();
	return enter_page(page_frame, target, flags);
}

/*
 * Allocate enough pages to contain the range of virtual addresses specified.
 */
uintptr_t alloc_pages(uintptr_t begin_addr, uintptr_t end_addr, int flags) {
	unsigned int first_page, last_page;
	first_page = begin_addr / PAGE_SIZE;
	last_page = (end_addr - 1) / PAGE_SIZE + 1;
	for (unsigned int i = first_page; i <= last_page; i++) {
		if (page_present(i * PAGE_SIZE)) {
			continue;
		}
		uintptr_t page_frame = alloc_page_frame();
		map_page(page_frame, i * PAGE_SIZE, flags);
	}

	return begin_addr / PAGE_SIZE * PAGE_SIZE;
}

/* 
 * Free the page frame and mark the virtual address as not present 
 * in de page table.
 */
void free_page(uintptr_t page) {
	free_page_frame(page_frame(page));
	free_address(page);
}

/* 
 * Do the same as 'map_page', but use the first free virtual address.
 */
uintptr_t enter_page(uintptr_t page_frame, uintptr_t target, int flags) {
	uintptr_t address = get_free_address(target);
	return map_page(page_frame, address, flags);
}

/* 
 * Mark the page that corresponds with the address as not present.
 */
void free_address(uintptr_t address) {
	uint32_t *page_entry = get_page_entry(address);
	if (page_entry != NULL)
		*page_entry &= ~1;
	vm_page_inval(address);
}

/* 
 * Map the specified page frame to the virtual address and return the virtal address.
 * Be carefull though, it doesn't care whether or not the address is already taken.
 */
uintptr_t map_page(uintptr_t page_frame, uintptr_t virtual_address, int flags) {
	uint32_t *page_entry;
	if (get_page_entry(virtual_address) == NULL) {
		add_page_table(virtual_address, flags);
	}

	page_entry = get_page_entry(virtual_address);
	*page_entry = (page_frame & 0xFFFFF000) | flags | 1;
	return virtual_address;
}

/* 
 * Create a new address space for a newly forked process.
 */
uint32_t *fork_mem(uint32_t *orig_page_dir) {
	uint32_t *new_dir = (uint32_t *)alloc_page(KERNEL_MEMORY, PG_KERN | PG_RW);
	if (new_dir < KERNEL_MEMORY)
		panic("Out of kernel memory.");

	memcpy(new_dir, orig_page_dir, PAGE_SIZE);
	
	for (unsigned int i = 0; i < dir_index(KERNEL_MEMORY); i++) {
		new_dir[i] &= ~2;
		new_dir[i] |= 0x400; /* Use bit 10 the indicate CoW for page 
														dir entries to the page fault handler. */
		orig_page_dir[i] &= ~2;
		orig_page_dir[i] |= 0x400;
	}
	new_dir[1023] &= ~2; /* The kernelspace shouldn't be CoW, except for the page tables. */
	new_dir[1023] |= 0x400;
	orig_page_dir[1023] &= ~2;
	orig_page_dir[1023] |= 0x400;

	inc_phys_refs(0, (uintptr_t)0 - PAGE_SIZE);

	return new_dir;
}
/* 
 * Return the page table entry used for a certain virtual address. 
 */
uint32_t *get_page_entry(uintptr_t virtual_address) {
	uint32_t page_dir_entry = current->page_directory[dir_index(virtual_address)];
	if (!(page_dir_entry & 1))
		return NULL;
	uint32_t *page_table = (uint32_t *)get_page_table(virtual_address);
	uint32_t page_index = page_table_index(virtual_address);
	return &page_table[page_index];
}

/* 
 * Allocate a page table and add it to the page directory.
 * The parameter 'virtual_address' is an address of which the page entry
 * is held in the new page table.
 */
uintptr_t add_page_table(uintptr_t virtual_address, uint32_t flags) {
	uintptr_t page_frame = alloc_page_frame();
	uint32_t *page_dir = current->page_directory;
	page_dir[dir_index(virtual_address)] = (uint32_t) page_frame | flags | 1;
	return get_page_table(virtual_address);
}

unsigned int dir_index(uintptr_t virtual_address) {
	return virtual_address >> 22;
}

unsigned int page_table_index(uintptr_t virtual_address) {
	return (virtual_address >> 12) & 0x3ff;
}

/*
 * Walk the page directory and page tables in order to find a 
 * free virtual address and return the address.
 */
uintptr_t get_free_address(uintptr_t target) {
	uint32_t index_offset, page_dir_entry, index, pt_index;
	index_offset = dir_index(target);
	pt_index = page_table_index(target);

	for (index = 0; index < 1023; index++) {
		uint32_t pd_index = (index+index_offset) % 1024;
		page_dir_entry = current->page_directory[pd_index];
		if (!(page_dir_entry & 1)) {       /* If the page table isn't present, the */
			add_page_table(pd_index << 22, 3);  /* first page will always be present */
			return pd_index << 22; 
		}

		uint32_t *page_table = (uint32_t *)get_page_table(pd_index << 22);
		for (;pt_index < 1024; pt_index++)
			if (!(page_table[pt_index] & 1))
				return pd_index << 22 | pt_index << 12;

		pt_index = 0;
	}

	panic("*** PANIC ***\nNo free virtual addresses available");
}

uintptr_t page_frame(uintptr_t page) {
	uint32_t *page_entry;
	if ((page_entry = get_page_entry(page)) == NULL)
		return NULL;
	return *page_entry & 0xFFFFF000;
}

uintptr_t get_page_table(uintptr_t virtual_address) {
	return (uintptr_t)(0 - PAGE_SIZE * (1024 - dir_index(virtual_address)));
}

bool page_present(uintptr_t virtual_address) {
	uint32_t *page_entry = get_page_entry(virtual_address);
	if (page_entry == NULL)
		return false;
	else if (*page_entry & 1)
		return true;
	else
		return false;
}

void enable_CoW(uint32_t *table) {
	for (int i = 0; i < 1024; i++) {
		table[i] &= ~2;
		table[i] |= 0x200; /* Use bit 9 to indicate CoW for page
													table entries to the page fault handler. */
	}
}

bool CoW_enabled(uintptr_t addr) {
	return current->page_directory[dir_index(addr)] & 0x400
		|| (*get_page_entry(addr)) & 0x200;
}

void pf_non_present(uint32_t addr) {
	if (addr >= current->heap_start && addr < current->heap_end) {
		alloc_page(addr, PG_USER | PG_RW);
	} else {
		panic("*** PAGE FAULT ***\nTrying to reference non existing page.");
	}
}

void pf_rsvd() {
	panic("*** PAGE FAULT ***\n1 writen to reserved field");
}

void pf_ro_violation(uint32_t addr) {
	uint32_t *page_table = (uint32_t *)get_page_table(addr);
	uintptr_t tmp, new_pf;

	if (addr >= current->code_start && addr <= current->code_end)
		panic("*** PAGE FAULT ***\nWrote to code segment.");

	if (!CoW_enabled(addr))
		panic("*** PAGE FAULT ***\nRead only violation");

	if (current->page_directory[dir_index(addr)] & 0x400) { /* CoW is enabled for
                                                      this page table entry. */
		enable_CoW(page_table);
		current->page_directory[dir_index(addr)] |= 2;
		current->page_directory[dir_index(addr)] &= ~0x400;
	} 

	tmp = enter_page(page_frame(addr), KERNEL_MEMORY, PG_KERN | PG_RW);
	if (tmp == NULL)
		panic("Not enough memory.");
	new_pf = alloc_page_frame();
	map_page(new_pf, addr, PG_USER | PG_RW);
	memcpy((void *)(addr / PAGE_SIZE * PAGE_SIZE), (void *)tmp, PAGE_SIZE);
	free_page(tmp);
}

void page_fault(Registers regs, uint32_t err) {
	uint32_t cr2 = read_cr2();

	if ((err & 1) == 0) {
		pf_non_present(cr2);
	} else if (err & 8) {
		pf_rsvd();
	} else if (err & 1) {
		pf_ro_violation(cr2);
	}	else if (err & 2) {
		panic("*** PAGE FAULT ***\nPage protection violation");
	} else {
		panic("*** PAGE FAULT ***\nCR2: %x, error code: %x", cr2, err);
	}
}
