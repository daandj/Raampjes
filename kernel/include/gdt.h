#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

#define GDT_SIZE 6

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

void init_gdt();

#endif
