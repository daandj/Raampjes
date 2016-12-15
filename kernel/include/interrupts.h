#include <stdint.h>

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
#define STACK_SEGMENT_FAULT	12
#define GPF                 13
#define PAGE_FAULT          14
#define FPU_EXC             16
#define ALIGNMENT_CHECK     17
#define MACHINE_CHECK       18
#define SIMD_FP_EXC         19
#define VIRTUALIZATION_EXC  20

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

typedef void (*intr_handler)(void);

int init_interrupts();
int enable_interrupts();
int disable_interrupts();
