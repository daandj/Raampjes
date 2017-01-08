#include <vga.h>
#include <interrupts.h>
#include <PIC.h>
#include <keyboard.h>
#include <stdint.h>
#include <mm.h>

int kmain(struct MMap *map, uint16_t mmap_size) {
	init_vga();
	init_interrupts();
	keyboard_init();
	PIC_init();
	init_mm();
	enable_interrupts();
	while (1) ;
	return 0;
}
