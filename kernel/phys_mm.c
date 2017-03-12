#include <stdbool.h>
#include <stdint.h>
#include <raampjes/mm.h>

#define WORD_SIZE     32
#define BITMAP_SIZE   (1024*1024/WORD_SIZE)

uint32_t bitmap[BITMAP_SIZE];
int search_bitmap(bool value, int start);
void set_bitmap(bool value, int index);
bool get_bitmap(int index);
void set_bitmap_area(bool value, int start, int end);

/* 
 * Initialize the physical memory manager, by updating the bitmap to
 * the current situation and mark sure unusable parts of the memory won't
 * be used.
 */
void init_phys_mm(struct MMap *map, uint16_t mmap_size) {
	int i, first_page, last_page;

	/* Mark entire physical address space as not locatable. */
	set_bitmap_area(1, 0, BITMAP_SIZE);
	
	/* Read the memory map into the bitmap */
	for (i = 0; i < mmap_size; i++) {
		if (map[i].region_type == AVAILABLE || 
				map[i].region_type == ACPI_reclaimable) {
			first_page = (map[i].base_address - 1)/PAGE_SIZE + 1;  /* The pluses and
											 minuses are to make sure a page of wich only half is 
											 available isn't mark as allocatable. */
			last_page = (map[i].length + map[i].base_address - 1)/PAGE_SIZE + 1;

			set_bitmap_area(0, first_page, last_page);
		}
	}

	/* Set overlapping areas to the most restricting type. */
	for (i = 0; i < mmap_size; i++) {
		if (map[i].region_type != AVAILABLE &&
				map[i].region_type != ACPI_reclaimable) {
			first_page = map[i].base_address / PAGE_SIZE;
			last_page = (map[i].length + map[i].base_address) / PAGE_SIZE;

			set_bitmap_area(1, first_page, last_page);
		}
	}

	/* Mark first 1024 page frames as used (BIOS data and kernel). */
	set_bitmap_area(1, 0, 1024);
}

/* 
 * Find a free page frame from the bitmap, mark it as used and
 * return its address. If no free page frames left, return NULL (0).
 */
uintptr_t alloc_page_frame() {
	int page_frame;
	if ((page_frame = search_bitmap(0, 0)) == NULL) {
		return NULL;
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
	return NULL;
}

void set_bitmap(bool value, int index) {
	bitmap[index/WORD_SIZE] ^= (-value ^ bitmap[index/WORD_SIZE]) & (1 << index);
}

bool get_bitmap(int index) {
	return (bitmap[index/WORD_SIZE] >> index % WORD_SIZE) & 1;
}

void set_bitmap_area(bool value, int start, int end) {
	int j;
	for (j = start; j < end; j++) {
		if (j > BITMAP_SIZE)
			continue;
		set_bitmap(value, j);
	}
}
