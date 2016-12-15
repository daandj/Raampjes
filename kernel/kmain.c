#include <stdint.h>
#include <vga.h>
#include <stdlib.h>
#include <interrupts.h>

int kmain() {
	init_vga();
	kprintf("Hello world!\n");
	kprintf("Hello again test\n");
	init_interrupts();
	kprintf("IDT initialized\n");
	enable_interrupts();
	while (1) ;
	return 0;
}
