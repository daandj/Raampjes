#include <stdbool.h>
#include <stdint.h>
#include <mm.h>

#define WORD_SIZE     32
#define BITMAP_SIZE   (1024*1024/WORD_SIZE)

uint32_t bitmap[BITMAP_SIZE];
int search_bitmap(bool value, int start);
void set_bitmap(bool value, int index);
bool get_bitmap(int index);

/* 
 * Initialize the physical memory manager, by updating the bitmap to
 * the current situation and mark sure unusable parts of the memory won't
 * be used.
 */
void init_phys_mm(struct MMap *map, uint16_t mmap_size) {
	int i, j, first_page, last_page;

	/* Mark entire physical address space as not locatable. */
	for (i = 0; i < BITMAP_SIZE; i++)
		set_bitmap(1, i);
	
	/* Read the memory map into the bitmap */
	for (i = 0; i < mmap_size; i++) {
		if (map[i].region_type == AVAILABLE || 
				map[i].region_type == ACPI_reclaimable) {
			first_page = (map[i].base_address - 1)/PAGE_SIZE + 1;  /* The pluses and
											 minuses are to make sure a page of wich only half is 
											 available isn't mark as allocatable. */
			last_page = (map[i].length - 1)/PAGE_SIZE + 1;
			for (j = first_page; j < last_page; j++)
				set_bitmap(0, j);
		}
	}
	/* Mark first page_table as used (BIOS data and kernel). */
	for (i = 0; i < 1024; i++)
		set_bitmap(1, i);
}

/* 
 * Find a free page frame from the bitmap, mark it as used and
 * return its address. If no free page frames left, return NOT_FOUND (-1).
 */
uintptr_t alloc_page_frame() {
	int page_frame;
	if ((page_frame = search_bitmap(0, 0)) == NOT_FOUND) {
		return NOT_FOUND;
	}
	set_bitmap(1, page_frame);
	return PAGE_SIZE * page_frame;
}

/* 
 * Mark the page frame as free in the bitmap.
 */
void free_page_frame(uintptr_t page_frame) {
	set_bitmap(0, page_frame / PAGE_SIZE);
}

int search_bitmap(bool value, int start) {
	int size = WORD_SIZE * BITMAP_SIZE;
	for (int i = start; i < size; i++) 
		/* TODO: Optimize this by comparing 32 bits at a time. */
		if (get_bitmap(i) == value)
			return i;
	return NOT_FOUND;
}

void set_bitmap(bool value, int index) {
	bitmap[index/WORD_SIZE] ^= (-value ^ bitmap[index/WORD_SIZE]) & (1 << index);
}

bool get_bitmap(int index) {
	return (bitmap[index/WORD_SIZE] >> index % WORD_SIZE) & 1;
}
