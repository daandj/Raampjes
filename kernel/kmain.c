#include <stdint.h>
#include <vga.h>

int kmain() {
	init_vga();
	kprintf("Hello world!\n");
	kprintf("Hello again test\n");
	return 0;
}
