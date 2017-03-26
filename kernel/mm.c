#include <stdint.h>
#include <stdbool.h>
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
uintptr_t page_frame(uintptr_t page);
void page_fault_handler(void);
uintptr_t get_page_table(uintptr_t virtual_address);
bool page_present(uintptr_t virtual_address);
void page_fault(Registers regs, uint32_t error_code);

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
 * Return the page table entry used for a certain virtual address. 
 */
uint32_t *get_page_entry(uintptr_t virtual_address) {
	uint32_t page_dir_entry = PageDirectory[dir_index(virtual_address)];
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
	PageDirectory[dir_index(virtual_address)] = (uint32_t) page_frame | flags | 1;
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
		page_dir_entry = PageDirectory[pd_index];
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

void page_fault(Registers regs, uint32_t error_code) {
	uint32_t cr2 = read_cr2();

	if (error_code & 1) {
		panic("*** PANIC ***\nPage protection violation");
	} else if (cr2 >= current->heap_start && cr2 < current->heap_end) {
		alloc_page(cr2, PG_USER | PG_RW);
		return;
	}

	panic("*** PAGE FAULT ***\nCR2: %x, error code: %x", cr2, error_code);
}

