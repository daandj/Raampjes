#include <stdint.h>
#include <raampjes/PIC.h>
#include <raampjes/vga.h>
#include <raampjes/cpu.h>

#define PIC_EOI 								0x20
#define ICW1										0x11
#define ICW2_MASTER							0x20
#define ICW2_SLAVE							0x28
#define ICW3_MASTER							0x04
#define ICW3_SLAVE							0x02
#define ICW4										0x01

void PIC_init() {
	outb(PIC_COMMAND_MASTER, ICW1);
	outb(PIC_COMMAND_SLAVE, ICW1);

	outb(PIC_DATA_MASTER, ICW2_MASTER);
	outb(PIC_DATA_SLAVE, ICW2_SLAVE);

	outb(PIC_DATA_MASTER, ICW3_MASTER);
	outb(PIC_DATA_SLAVE, ICW3_SLAVE);

	outb(PIC_DATA_MASTER, ICW4);
	outb(PIC_DATA_SLAVE, ICW4);
}

void disable_PIC() {
	uint8_t masks = 0xFF;
	outb(PIC_DATA_MASTER, masks);
	outb(PIC_DATA_SLAVE, masks);
}

void mask_IRQ(int irq) {
	uint8_t masks;
	uint16_t port;
	if (irq > 7) {
		irq -= 8;
		port = PIC_DATA_SLAVE;
	} else
		port = PIC_DATA_MASTER;
	masks = inb(port);
	outb(port, (masks | 1 << irq));
}

void send_PIC_EOI(uint32_t irq) {
	if (irq >= 8)
		outb(PIC_COMMAND_SLAVE, PIC_EOI);
	outb(PIC_COMMAND_MASTER, PIC_EOI);
}
