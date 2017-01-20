#include <stdint.h>

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);
void kernel_halt();
void vm_page_inval(uintptr_t address);
