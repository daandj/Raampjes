#include <stdbool.h>
#include <stdint.h>
#include <raampjes/panic.h>
#include <raampjes/mm.h>

#define BITMAP_SIZE   (1024*1024)

static uint8_t bitmap[BITMAP_SIZE];
int search_bitmap(bool value, int start);
void set_bitmap(bool value, int index);
int get_bitmap(int index);
void set_bitmap_area(bool value, int start, int end);
void increase_bitmap(unsigned int index);
void decrease_bitmap(unsigned int index);
void increase_bitmap_area(int start, int end);
void decrease_bitmap_area(int start, int end);

/*
 * Initialize the physical memory manager, by updating the bitmap to
 * the current situation and mark sure unusable parts of the memory won't
 * be used.
 */
void init_phys_mm(struct multiboot_tag_mmap *map_tag) {
	int i, first_page, last_page;
	multiboot_memory_map_t *map = VIRTUAL_ADDRESS(map_tag->entries);

	/* Mark entire physical address space as not locatable. */
	set_bitmap_area(1, 0, BITMAP_SIZE);

	/* Read the memory map into the bitmap */
	for (i = 0; (uintptr_t)&map[i] < (uintptr_t)map_tag + map_tag->size; i++) {
		if (map[i].type == MULTIBOOT_MEMORY_AVAILABLE ||
				map[i].type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
			first_page = (map[i].addr)/PAGE_SIZE;
			last_page = (map[i].len + map[i].addr)/PAGE_SIZE;

			set_bitmap_area(0, first_page, last_page);
		}
	}

	/* Set overlapping areas to the most restricting type. */
	for (i = 0; (uintptr_t)&map[i] < (uintptr_t)map_tag + map_tag->size; i++) {
		if (map[i].type != MULTIBOOT_MEMORY_AVAILABLE &&
				map[i].type != MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
			first_page = map[i].addr / PAGE_SIZE;
			last_page = (map[i].len + map[i].addr) / PAGE_SIZE;

			set_bitmap_area(1, first_page, last_page);
		}
	}

	/* Mark first 1024 page frames as used (BIOS data and kernel). */
	set_bitmap_area(1, 0, 1024);
}

/*
 * Find a free page frame from the bitmap, mark it as used and
 * return its address. If no free page frames left, return (-1).
 */
uintptr_t alloc_page_frame() {
	int page_frame;
	if ((page_frame = search_bitmap(0, 0)) == -1) {
		panic("Out of memory.\n");
		return NULL;
	}
	set_bitmap(1, page_frame);
	return PAGE_SIZE * page_frame;
}

/*
 * Mark the page frame as free in the bitmap.
 */
void free_page_frame(uintptr_t page_frame) {
	if (get_bitmap(page_frame / PAGE_SIZE))
		decrease_bitmap(page_frame / PAGE_SIZE);
}

/*
 * Increase the amount of references counted in the bitmap.
 */
void inc_phys_refs(uintptr_t start, uintptr_t end) {
	unsigned int first_page = start / PAGE_SIZE;
	unsigned int final_page = end / PAGE_SIZE;
	uintptr_t pf;

	for (unsigned int i = first_page; i < final_page; i++) {
		if ((pf = page_frame(i * PAGE_SIZE)) != NULL)
			increase_bitmap(pf / PAGE_SIZE);
	}
}


int search_bitmap(bool value, int start) {
	int size = BITMAP_SIZE;
	for (int i = start; i < size; i++)
		if (get_bitmap(i) == value)
			return i;
	return -1;
}

inline void set_bitmap(bool value, int index) {
	bitmap[index] = value;
}

inline int get_bitmap(int index) {
	return bitmap[index];
}

void set_bitmap_area(bool value, int start, int end) {
	int j;
	for (j = start; j < end; j++) {
		if (j > BITMAP_SIZE)
			continue;
		set_bitmap(value, j);
	}
}

inline void increase_bitmap(unsigned int index) {
	if (index > BITMAP_SIZE)
		panic("*** PANIC ***\n"__FILE__":%d", __LINE__);
	bitmap[index]++;
}

inline void decrease_bitmap(unsigned int index) {
	if (index > BITMAP_SIZE)
		panic("*** PANIC ***\n"__FILE__ ":%d", __LINE__);
	bitmap[index]--;
}
