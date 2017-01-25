#include <stdint.h>
#include <cpu.h>
#include <interrupts.h>

#define READ_CR(x) inline unsigned long read_cr ## x(void) { \
	unsigned long val; \
	asm volatile ( "mov %%cr" # x ", %0" : "=r"(val) ); \
	return val; \
}

inline void outb(uint16_t port, uint8_t data) {
	asm ("outb %0, %1" : : "a"(data), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
	uint8_t _return;
	asm ("inb %1, %0" : "=a"(_return) : "Nd"(port));
	return _return;
}

void kernel_halt() {
	disable_interrupts();
	asm ("hlt");
}

inline void vm_page_inval(uintptr_t address) {
	asm volatile ("invlpg (%0)" :: "a" (address));
}

READ_CR(0)
READ_CR(1)
READ_CR(2)
READ_CR(3)
READ_CR(4)
