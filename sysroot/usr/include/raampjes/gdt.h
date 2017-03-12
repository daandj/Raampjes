#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

#define GDT_SIZE        6
#define SEG_ZERO        0x0
#define SEG_KERNEL_CODE 0x8
#define SEG_KERNEL_DATA 0x10
#define MAIN_TSS_DESC   0x1B
#define SEG_USER_CODE   0x23
#define SEG_USER_DATA   0x2B

struct GDT_entry {
	unsigned int limit_low : 16;
	unsigned int base_low : 16;
	unsigned int base_middle : 8;
	unsigned int access : 8;
	unsigned int limit_high : 4;
	unsigned int flags : 4;
	unsigned int base_high : 8;
} __attribute__((__packed__));

struct GDTR {
	uint16_t size;
	uint32_t offset;
} __attribute__((__packed__));

#define CREATE_DESC(base, limit, flags, access) \
(struct GDT_entry){ \
	(limit) & 0xFFFF, (base) & 0xFFFF, ((base) >> 16) & 0xFF, (access), \
	((limit) >> 16) & 0x0F, (flags) & 0x0C, (base) >> 24 \
}

void init_gdt();

#endif
