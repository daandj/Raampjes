#include <gdt.h>
#include <stdint.h>
#include <cpu.h>

#define SEG_DATA_RD        0x90 // Read-Only
#define SEG_DATA_RDWR      0x92 // Read/Write
#define SEG_CODE_EX        0x98 // Execute-Only
#define SEG_CODE_EXRD      0x9A // Execute/Read
#define SEG_SIZE_16        0x08 // 16bit descriptor
#define SEG_SIZE_32        0x0C // 32bit descriptor

#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)

#define CREATE_DESC(base, limit, flags, access) \
(struct GDT_entry){ \
	(limit) & 0xFFFF, (base) & 0xFFFF, ((base) >> 16) & 0xFF, (access), \
	((limit) >> 16) & 0x0F, (flags) & 0x0C, (base) >> 24 \
}

void load_gdt();

struct GDT_entry GDT[GDT_SIZE];
struct GDTR gdtr;

void init_gdt() {
	GDT[0] = CREATE_DESC(0, 0, 0, 0);
	GDT[1] = CREATE_DESC(0, 0xFFFFF, SEG_SIZE_32, SEG_PRIV(0) | SEG_CODE_EXRD);
	GDT[2] = CREATE_DESC(0, 0xFFFFF, SEG_SIZE_32, SEG_PRIV(0) | SEG_DATA_RDWR);
	GDT[4] = CREATE_DESC(0, 0xFFFFF, SEG_SIZE_32, SEG_PRIV(3) | SEG_CODE_EXRD);
	GDT[5] = CREATE_DESC(0, 0xFFFFF, SEG_SIZE_32, SEG_PRIV(3) | SEG_DATA_RDWR);
	load_gdt();
}

void load_gdt() {
	gdtr.offset = GDT;
	gdtr.size = sizeof(GDT) - 1;
	lgdt(gdtr);
}
