#include <interrupts.h>
#include <stdint.h>
#include <stdlib.h>
#include <vga.h>
#include <PIC.h>
#include <panic.h>
#include <keyboard.h>

#define BIT32_INTR_GATE		0x8E
#define SIZE_IDT					256

int install_intr_handler(int, intr_handler);
int install_idt(uint16_t size, struct IDTDescriptor *_idt);
void main_irq_handler(uint32_t irq);

struct IDTDescriptor IDT[SIZE_IDT];

int init_interrupts() {
	disable_interrupts();
	install_intr_handler(ZERO_DIVIDE, 									intr0);
	install_intr_handler(DEBUG, 												intr1);
	install_intr_handler(NMI, 													intr2);
	install_intr_handler(BREAKPOINT, 										intr3);
	install_intr_handler(OVERFLOW, 											intr4);
	install_intr_handler(BOUNDS_CHECK, 									intr5);
	install_intr_handler(INVALID_OPCODE, 								intr6);
	install_intr_handler(DEVICE_UNAVAILABLE, 						intr7);
	install_intr_handler(DOUBLE_FAULT, 									intr8);
	install_intr_handler(COPROCESSOR_SEGMENT_OVERRUN, 	intr9);
	install_intr_handler(INVALID_TSS, 									intr10);
	install_intr_handler(SEGMENT_NOT_PRESENT, 					intr11);
	install_intr_handler(STACK_SEGMENT_FAULT, 					intr12);
	install_intr_handler(GPF, 													intr13);
	install_intr_handler(PAGE_FAULT, 										intr14);
	install_intr_handler(FPU_EXC, 											intr16);
	install_intr_handler(ALIGNMENT_CHECK, 							intr17);
	install_intr_handler(MACHINE_CHECK, 								intr18);
	install_intr_handler(SIMD_FP_EXC, 									intr19);
	install_intr_handler(VIRTUALIZATION_EXC, 						intr20);
	install_intr_handler(IRQ0,													intr32);
	install_intr_handler(IRQ1,													intr33);
	install_intr_handler(IRQ2,													intr34);
	install_intr_handler(IRQ3,													intr35);
	install_intr_handler(IRQ4,													intr36);
	install_intr_handler(IRQ5,													intr37);
	install_intr_handler(IRQ6,													intr38);
	install_intr_handler(IRQ7,													intr39);
	install_intr_handler(IRQ8,													intr40);
	install_intr_handler(IRQ9,													intr41);
	install_intr_handler(IRQ10,													intr42);
	install_intr_handler(IRQ11,													intr43);
	install_intr_handler(IRQ12,													intr44);
	install_intr_handler(IRQ13,													intr45);
	install_intr_handler(IRQ14,													intr46);
	install_intr_handler(IRQ15,													intr47);
	install_idt(SIZE_IDT, IDT);
	return 0;
}

int enable_interrupts() {
	asm ( "sti" );
	return 0;
}

int disable_interrupts() {
	asm ( "cli" );
	return 0;
}

int install_intr_handler(int intr_line, intr_handler handler) {
	struct IDTDescriptor *idt_entry = &IDT[intr_line];
	idt_entry->offset_low = (uint16_t) handler & 0xFFFF;
	idt_entry->offset_high = (uint16_t) ((int) handler >> 16) & 0xFFFF;
	idt_entry->zero = 0;
	idt_entry->selector = 0x08;				// 0x08 is the GDT entry for the kernel code segment.
	idt_entry->type_attr = BIT32_INTR_GATE;
	return 0;
}

void main_intr_handler(uint32_t interrupt_number, struct Registers regs, uint32_t error_code) {
	if ((interrupt_number >= 32) && (interrupt_number < 48)) {
		main_irq_handler(interrupt_number-32);
		return;
	};
	
	panic("EAX: %x, EBX: %x, ECX: %x\nEDX: %x, ESP: %x, EBP: %x\n\
ESI: %x, EDI: %x;\nInterrupt number: %x;\nError code: %x;",
			regs.eax, regs.ebx, regs.ecx,	regs.edx,
			regs.esp, regs.ebp, regs.esi, regs.edi,
			interrupt_number, error_code);
}

int install_idt(uint16_t size, struct IDTDescriptor *_idt) {
	struct IDTR *_idtr;
	_idtr->limit = (8 * size - 1); 
	_idtr->base = (uint32_t) _idt;
	asm ( "lidt %0" : : "m"(*_idtr) );
	return 0;
}

void main_irq_handler(uint32_t irq_number) {
	switch (irq_number) {
		case 0:
			break;
		case 1:
			handle_keyboard_input();
			break;
		default:
			kprintf("%u", irq_number);
			break;
	}
	send_PIC_EOI(irq_number);
}
