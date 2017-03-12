#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>
#include <raampjes/gdt.h>
#include <raampjes/interrupts.h>

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);
void lgdt(struct GDTR gdtr);
void lidt(struct IDTR idtr);
void ltr(uint16_t task_desc);
void kernel_halt();
void vm_page_inval(uintptr_t address);
unsigned long read_cr0(void);
unsigned long read_cr2(void);
unsigned long read_cr3(void);
unsigned long read_cr4(void);

#endif
