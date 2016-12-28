#include <vga.h>
#include <interrupts.h>
#include <PIC.h>
#include <keyboard.h>

int kmain() {
	init_vga();
	kprintf("Hello world!\n");
	init_interrupts();
	kprintf("IDT initialized\n");
	keyboard_init();
	PIC_init();
	enable_interrupts();
	while (1) ;
	return 0;
}
