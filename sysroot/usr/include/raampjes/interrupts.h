#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>

#define IDT_BIT32_GATE      0xe
#define IDT_BIT32_TRAP      0xf
#define IDT_DPL_3           0x60
#define IDT_DPL_0           0x0
#define IDT_PRESENT         0x80

#define ZERO_DIVIDE         0
#define DEBUG               1
#define NMI                 2
#define BREAKPOINT          3
#define OVERFLOW            4
#define BOUNDS_CHECK        5
#define INVALID_OPCODE      6
#define DEVICE_UNAVAILABLE  7
#define DOUBLE_FAULT        8
#define COPROCESSOR_SEGMENT_OVERRUN 9
#define INVALID_TSS         10
#define SEGMENT_NOT_PRESENT 11
#define STACK_SEGMENT_FAULT 12
#define GPF                 13
#define PAGE_FAULT          14
#define FPU_EXC             16
#define ALIGNMENT_CHECK     17
#define MACHINE_CHECK       18
#define SIMD_FP_EXC         19
#define VIRTUALIZATION_EXC  20
#define IRQ0                32
#define IRQ1                33
#define IRQ2                34
#define IRQ3                35
#define IRQ4                36
#define IRQ5                37
#define IRQ6                38
#define IRQ7                39
#define IRQ8                40
#define IRQ9                41
#define IRQ10               42
#define IRQ11               43
#define IRQ12               44
#define IRQ13               45
#define IRQ14               46
#define IRQ15               47
#define SYS_INTR            48

extern void intr0();
extern void intr1();
extern void intr2();
extern void intr3();
extern void intr4();
extern void intr5();
extern void intr6();
extern void intr7();
extern void intr8();
extern void intr9();
extern void intr10();
extern void intr11();
extern void intr12();
extern void intr13();
extern void intr14();
extern void intr16();
extern void intr17();
extern void intr18();
extern void intr19();
extern void intr20();
extern void intr32();
extern void intr33();
extern void intr34();
extern void intr35();
extern void intr36();
extern void intr37();
extern void intr38();
extern void intr39();
extern void intr40();
extern void intr41();
extern void intr42();
extern void intr43();
extern void intr44();
extern void intr45();
extern void intr46();
extern void intr47();
extern uint32_t syscall_handler();

struct IDTDescriptor {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_high;
}__attribute((__packed__));

struct IDTR {
	uint16_t limit;
	uint32_t base;
}__attribute((__packed__));

typedef struct Registers {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
}__attribute((__packed__)) Registers;

typedef void (*intr_handler)(void);
typedef void (*intr_callback)(Registers, uint32_t);

int init_interrupts();
int enable_interrupts();
int disable_interrupts();
int set_interrupt_callback(int interrupt, intr_callback handler);

#endif
