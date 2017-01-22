#include <stdint.h>
#include <stdbool.h>
#include <mm.h>
#include <vga.h>
#include <panic.h>
#include <cpu.h>
#include <interrupts.h>

extern uint32_t PageDirectory[1024];
/* 
 * Because the page directory entries stores the physical address
 * of the corresponding page table, and we need the virtual address
 * in order to change the page table entries it contains, we use this
 * mapping to translate the physical to virtual addresses.
 */
uintptr_t page_table_map[1024];

unsigned int page_dir_index(uintptr_t virtual_address);
unsigned int page_table_index(uintptr_t virtual_address);
uintptr_t get_free_address(uintptr_t target);
uintptr_t map_page(uintptr_t page_frame, uintptr_t virtual_address);
uintptr_t add_page_table(uintptr_t virtual_address);
uint32_t *get_page_entry(uintptr_t virtual_address);
uintptr_t page_frame(uintptr_t page);
void page_fault_handler(void);

/* 
 * Initialize the memory manager.
 */
void init_mm(struct MMap *map, uint16_t mmap_size) {
	init_phys_mm(map, mmap_size);
	free_page(PAGE_SIZE * 1023);
	extern uint32_t *PageTable1;
	page_table_map[768] = (uintptr_t) PageTable1;

	/* Because were now fully operating in the higher half we don't
	 * need the first 4MiB to be mapped, so we free it. */
	for (int i = 0; i < 1024; i++) {
		free_address(i * PAGE_SIZE);
	}
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
	if (page_table_index(address) == 1023)
		add_page_table(address + PAGE_SIZE);
	address = get_free_address(virtual_address);
	return map_page(page_frame, address);
}

/* 
 * Mark the page that corresponds with the address as not present.
 */
void free_address(uintptr_t address) {
	uint32_t *page_entry = get_page_entry(address);
	*page_entry &= ~1;
	vm_page_inval(address);
}

/* 
 * Map the specified page frame to the virtual address and return the virtal address.
 * Be carefull though, it doesn't care whether or not the address is already taken.
 */
uintptr_t map_page(uintptr_t page_frame, uintptr_t virtual_address) {
	uint32_t *page_entry;
	if (get_page_entry(virtual_address) == NOT_FOUND)
		add_page_table(virtual_address);
	page_entry = get_page_entry(virtual_address);
	*page_entry = (page_frame & 0xFFFFF000) | 3;
	return virtual_address;
}

/* 
 * Return the page table entry used for a certain virtual address. 
 */
uint32_t *get_page_entry(uintptr_t virtual_address) {
	uint32_t page_dir_entry = PageDirectory[virtual_address >> 22];
	if (!(page_dir_entry & 1))
		return NOT_FOUND;
	uint32_t *page_table = (uint32_t *)page_table_map[virtual_address >> 22];
	uint32_t page_index = page_table_index(virtual_address);
	return &page_table[page_index];
}

/* 
 * Allocate a page table and add it to the page directory.
 * The parameter 'virtual_address' is an address of which the page entry
 * is held in the new page table.
 */
uintptr_t add_page_table(uintptr_t virtual_address) {
	uint32_t *page_dir_entry = &PageDirectory[virtual_address >> 22];

	if ((int)*page_dir_entry & 1) /* In case the page table is already present. */
		return (uintptr_t)get_page_entry(virtual_address);

	uintptr_t page_frame = alloc_page_frame();
	uintptr_t page_table_start = get_free_address(KERNEL_MEMORY);
	map_page(page_frame, page_table_start);
	*page_dir_entry = (page_frame & 0xFFFFF000) | 3;
	page_table_map[virtual_address >> 22] = page_table_start;

	uint32_t *page_table = (uint32_t *)page_table_start;
	for (int i = 0; i < 1024; i++) 
		page_table[i] = 0;
	return page_table_start;
}

unsigned int page_dir_index(uintptr_t virtual_address) {
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
	unsigned index_offset, page_dir_entry, index, table_index;
	index_offset = page_dir_index(target);
	table_index = page_table_index(target);

	for (index = 0; index < 1024; index++) {
		page_dir_entry = PageDirectory[(index+index_offset) % 1024];
		if (!(page_dir_entry & 1)) { /* Check if the page directory is marked present. */
			table_index = 0;
			continue;
		}
		uint32_t *page_table = (uint32_t *)page_table_map[(index+index_offset) % 1024];
		for (;table_index < 1024; table_index++) {
			if (!(page_table[table_index] & 1)) {
				return (uintptr_t)(page_table[table_index] & 0xFFFFF000);
			}
		}
		table_index = 0;
	}

	panic("*** PANIC ***\nNo free virtual addresses available");
}

uintptr_t page_frame(uintptr_t page) {
	uint32_t *page_entry;
	if ((page_entry = get_page_entry(page)) == NOT_FOUND)
		return NOT_FOUND;
	return *page_entry & 0xFFFFF000;
}
