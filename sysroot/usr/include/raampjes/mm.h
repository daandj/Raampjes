#ifndef _MM_H
#define _MM_H

#include <stdint.h>
#include <raampjes/multiboot2.h>

#define PAGE_SIZE     0x1000
#define NULL          (void *)0
#define KERNEL_MEMORY (uintptr_t)0xC0000000
#define VIRTUAL_ADDRESS(x) ((x)+KERNEL_MEMORY)
#define PG_USER       0x4
#define PG_KERN       0x0
#define PG_RW         0x2
#define PG_RO         0x0

void init_mm(struct multiboot_tag_mmap *map);
void init_phys_mm(struct multiboot_tag_mmap *map);
uintptr_t alloc_page(uintptr_t target, int flags);
uintptr_t alloc_pages(uintptr_t begin_addr, uintptr_t end_addr, int flags);
uintptr_t alloc_page_frame();
uintptr_t enter_page(uintptr_t page_frame, uintptr_t target, int flags);
uintptr_t map_page(uintptr_t page_frame, uintptr_t virtual_address, int flags);
void free_page(uintptr_t page);
void free_page_frame(uintptr_t page_frame);
void free_address(uintptr_t address);

#endif
