#include <stdint.h>
#include <stdbool.h>
#include <mm.h>
#include <panic.h>
#include <cpu.h>
#include <interrupts.h>

extern uint32_t PageDirectory[1024];
extern uint32_t PageTable1[1024];

unsigned int dir_index(uintptr_t virtual_address);
unsigned int page_table_index(uintptr_t virtual_address);
uintptr_t get_free_address(uintptr_t target);
uintptr_t map_page(uintptr_t page_frame, uintptr_t virtual_address);
uintptr_t add_page_table(uintptr_t virtual_address, uint32_t flags);
uint32_t *get_page_entry(uintptr_t virtual_address);
uintptr_t page_frame(uintptr_t page);
void page_fault_handler(void);
uintptr_t get_page_table(uintptr_t virtual_address);

/* 
 * Initialize the memory manager.
 */
void init_mm(struct MMap *map, uint16_t mmap_size) {
	init_phys_mm(map, mmap_size);
	/*
	 * We map the page directory to the last entry of itself, this way
	 * it maps all the present page tables to the last 1024 pages in the
	 * virtual address space.
	 */
	PageDirectory[1023] = ((uintptr_t)&PageDirectory - KERNEL_MEMORY) | 3;

	/* Because were now fully operating in the higher half we don't
	 * need the first 4MiB to be mapped, so we free it. */
	PageDirectory[0] &= ~1;
}

/*
 * Allocate a new page frame, put it at the 
 * first free virtual address, starting from the 'target' address and
 * return the virtual address.
 */
uintptr_t alloc_page(uintptr_t target) {
	uintptr_t page_frame = alloc_page_frame();
	return enter_page(page_frame, target);
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
uintptr_t enter_page(uintptr_t page_frame, uintptr_t virtual_address) {
	uintptr_t address = get_free_address(virtual_address);
	return map_page(page_frame, address);
}

/* 
 * Mark the page that corresponds with the address as not present.
 */
void free_address(uintptr_t address) {
	uint32_t *page_entry = get_page_entry(address);
	if (page_entry == NOT_FOUND)
		*page_entry &= ~1;
	vm_page_inval(address);
}

/* 
 * Map the specified page frame to the virtual address and return the virtal address.
 * Be carefull though, it doesn't care whether or not the address is already taken.
 */
uintptr_t map_page(uintptr_t page_frame, uintptr_t virtual_address) {
	uint32_t *page_entry;
	if (get_page_entry(virtual_address) == NOT_FOUND) {
		int flags = 0;
		add_page_table(virtual_address, flags);
	}

	page_entry = get_page_entry(virtual_address);
	*page_entry = (page_frame & 0xFFFFF000) | 3;
	return virtual_address;
}

/* 
 * Return the page table entry used for a certain virtual address. 
 */
uint32_t *get_page_entry(uintptr_t virtual_address) {
	uint32_t page_dir_entry = PageDirectory[dir_index(virtual_address)];
	if (!(page_dir_entry & 1))
		return NOT_FOUND;
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
	if ((page_entry = get_page_entry(page)) == NOT_FOUND)
		return NOT_FOUND;
	return *page_entry & 0xFFFFF000;
}

uintptr_t get_page_table(uintptr_t virtual_address) {
	return (uintptr_t)(0 - PAGE_SIZE * (1024 - dir_index(virtual_address)));
}
