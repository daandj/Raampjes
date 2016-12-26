#include <stdint.h>
#include <cpu.h>
#include <interrupts.h>

void outb(uint16_t port, uint8_t data) {
	asm ("outb %0, %1" : : "a"(data), "Nd"(port));
}

uint8_t inb(uint16_t port) {
	uint8_t _return;
	asm ("inb %1, %0" : "=a"(_return) : "Nd"(port));
	return _return;
}

void kernel_halt() {
	disable_interrupts();
	asm ("hlt");
}
