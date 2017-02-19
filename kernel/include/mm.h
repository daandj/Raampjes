#ifndef _MM_H
#define _MM_H

#include <stdint.h>

#define PAGE_SIZE     0x1000
#define NOT_FOUND     -1
#define KERNEL_MEMORY (uintptr_t)0xC0000000

enum REGION_TYPE { AVAILABLE = 1, RESERVED, ACPI_reclaimable, ACPI_NVS, BAD_MEM };

struct MMap {
	uint64_t base_address;
	uint64_t length;
	uint32_t region_type;
	uint32_t ACPI_attributes;
}__attribute__((__packed__));

void init_mm(struct MMap *map, uint16_t mmap_size);
void init_phys_mm(struct MMap *map, uint16_t mmap_size);
uintptr_t alloc_page(uintptr_t target);
uintptr_t alloc_page_frame();
uintptr_t enter_page(uintptr_t page_frame, uintptr_t target);
void free_page(uintptr_t page);
void free_page_frame(uintptr_t page_frame);
void free_address(uintptr_t address);

#endif
