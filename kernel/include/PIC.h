#include <stdint.h>
#define PIC_COMMAND_MASTER 			0x20
#define PIC_DATA_MASTER					0x21
#define PIC_COMMAND_SLAVE				0xA0
#define PIC_DATA_SLAVE					0xA1

void PIC_init();
void disable_PIC();
void mask_IRQ(int irq);
void send_PIC_EOI(uint32_t irq);
