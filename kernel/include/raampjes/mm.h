#ifndef _MM_H
#define _MM_H

#include <stdint.h>

#define PAGE_SIZE     0x1000
#define NULL          (void *)0
#define KERNEL_MEMORY (uintptr_t)0xC0000000
#define VIRTUAL_ADDRESS(x) ((x)+KERNEL_MEMORY)
#define PG_USER       0x4
#define PG_KERN       0x0
#define PG_RW         0x2
#define PG_RO         0x0

enum REGION_TYPE { AVAILABLE = 1, RESERVED, ACPI_reclaimable, ACPI_NVS, BAD_MEM };

struct MMap {
	uint64_t base_address;
	uint64_t length;
	uint32_t region_type;
	uint32_t ACPI_attributes;
}__attribute__((__packed__));

void init_mm(struct MMap *map, uint16_t mmap_size);
void init_phys_mm(struct MMap *map, uint16_t mmap_size);
uintptr_t alloc_page(uintptr_t target, int flags);
uintptr_t alloc_pages(uintptr_t begin_addr, uintptr_t end_addr, int flags);
uintptr_t alloc_page_frame();
uintptr_t enter_page(uintptr_t page_frame, uintptr_t target, int flags);
uintptr_t map_page(uintptr_t page_frame, uintptr_t virtual_address, int flags);
void free_page(uintptr_t page);
void free_page_frame(uintptr_t page_frame);
void free_address(uintptr_t address);

#endif
