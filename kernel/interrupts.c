#include <raampjes/interrupts.h>
#include <stdint.h>
#include <raampjes/stdlib.h>
#include <raampjes/vga.h>
#include <raampjes/PIC.h>
#include <raampjes/panic.h>
#include <raampjes/keyboard.h>
#include <raampjes/cpu.h>

#define SIZE_IDT					256

int install_intr_handler(int, intr_handler, int);
int install_idt(uint16_t size, struct IDTDescriptor *_idt);
void main_irq_handler(Registers, uint32_t, uint32_t irq);

struct IDTDescriptor IDT[SIZE_IDT];
struct IDTR _idtr;
intr_callback callbacks[SIZE_IDT];

int init_interrupts() {
	for (int i = 0; i < SIZE_IDT; i++)
		callbacks[i] = 0;
	for (int i = 32; i < 48; i++)
		callbacks[i] = &main_irq_handler;

	int flags = IDT_BIT32_TRAP | IDT_PRESENT | IDT_DPL_0;

	disable_interrupts();
	install_intr_handler(ZERO_DIVIDE, 									intr0, flags);
	install_intr_handler(DEBUG, 												intr1, flags);
	install_intr_handler(NMI, 													intr2, flags);
	install_intr_handler(BREAKPOINT, 										intr3, flags);
	install_intr_handler(OVERFLOW, 											intr4, flags);
	install_intr_handler(BOUNDS_CHECK, 									intr5, flags);
	install_intr_handler(INVALID_OPCODE, 								intr6, flags);
	install_intr_handler(DEVICE_UNAVAILABLE, 						intr7, flags);
	install_intr_handler(DOUBLE_FAULT, 									intr8, flags);
	install_intr_handler(COPROCESSOR_SEGMENT_OVERRUN, 	intr9, flags);
	install_intr_handler(INVALID_TSS, 									intr10, flags);
	install_intr_handler(SEGMENT_NOT_PRESENT, 					intr11, flags);
	install_intr_handler(STACK_SEGMENT_FAULT, 					intr12, flags);
	install_intr_handler(GPF, 													intr13, flags);
	install_intr_handler(PAGE_FAULT, 										intr14, flags);
	install_intr_handler(FPU_EXC, 											intr16, flags);
	install_intr_handler(ALIGNMENT_CHECK, 							intr17, flags);
	install_intr_handler(MACHINE_CHECK, 								intr18, flags);
	install_intr_handler(SIMD_FP_EXC, 									intr19, flags);
	install_intr_handler(VIRTUALIZATION_EXC, 						intr20, flags);
	install_intr_handler(IRQ0,													intr32, flags);
	install_intr_handler(IRQ1,													intr33, flags);
	install_intr_handler(IRQ2,													intr34, flags);
	install_intr_handler(IRQ3,													intr35, flags);
	install_intr_handler(IRQ4,													intr36, flags);
	install_intr_handler(IRQ5,													intr37, flags);
	install_intr_handler(IRQ6,													intr38, flags);
	install_intr_handler(IRQ7,													intr39, flags);
	install_intr_handler(IRQ8,													intr40, flags);
	install_intr_handler(IRQ9,													intr41, flags);
	install_intr_handler(IRQ10,													intr42, flags);
	install_intr_handler(IRQ11,													intr43, flags);
	install_intr_handler(IRQ12,													intr44, flags);
	install_intr_handler(IRQ13,													intr45, flags);
	install_intr_handler(IRQ14,													intr46, flags);
	install_intr_handler(IRQ15,													intr47, flags);

	flags |= IDT_DPL_3;
	install_intr_handler(SYS_INTR, syscall_handler, flags);

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

int install_intr_handler(int intr_line, intr_handler handler, int flags) {
	struct IDTDescriptor *idt_entry = &IDT[intr_line];
	idt_entry->offset_low = (uint16_t) handler & 0xFFFF;
	idt_entry->offset_high = (uint16_t) ((int) handler >> 16) & 0xFFFF;
	idt_entry->zero = 0;
	idt_entry->selector = 0x08;				// 0x08 is the GDT entry for the kernel code segment.
	idt_entry->type_attr = flags;
	return 0;
}

void main_intr_handler(uint32_t interrupt_number, 
                       Registers regs, 
											 uint32_t error_code) {
	if (callbacks[interrupt_number]) {
		callbacks[interrupt_number](regs, error_code, interrupt_number);
		if (interrupt_number >= 32 && interrupt_number < 48)
			send_PIC_EOI(interrupt_number - 32);
		return;
	}
		
	panic("EAX: %x, EBX: %x, ECX: %x\nEDX: %x, ESP: %x, EBP: %x\n\
ESI: %x, EDI: %x, CR0: %x;\nCR2: %x, CR3: %x, CR4 %x\n\
Interrupt number: %x;\n Error code: %x;",
			regs.eax, regs.ebx, regs.ecx,	regs.edx,
			regs.esp, regs.ebp, regs.esi, regs.edi,
			read_cr0(), read_cr2(), read_cr3(), read_cr4(),
			interrupt_number, error_code);
}

int install_idt(uint16_t size, struct IDTDescriptor *_idt) {
	_idtr.limit = (8 * size - 1); 
	_idtr.base = (uint32_t) _idt;
	lidt(_idtr);
	return 0;
}

void main_irq_handler(Registers regs, uint32_t error_code, uint32_t irq_number) {
	return;
}

int set_interrupt_callback(int interrupt, intr_callback callback) {
	callbacks[interrupt] = callback;
	return 0;
}
