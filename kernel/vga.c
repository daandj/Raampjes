#include <stdint.h>
#include <vga.h>
#define VGA_WIDTH 80
#define VGA_HEIGTH 25

uint16_t vga_entry(char);

int VGA_x;
int VGA_y;
uint16_t *vga_memory_address;

int init_vga() {
	VGA_x = 0;
	VGA_y = 0;
	vga_memory_address = (uint16_t *) 0xb8000;
	return 0;
}

int kprintf(const char *format) {
	char *c = format;
	while (*c != 0) {
		if (*c == '\n') {
			VGA_x = 0;
			VGA_y++;
			c++;
			continue;
		}
		int index = VGA_y * VGA_WIDTH + VGA_x;
		vga_memory_address[index] = vga_entry(*c++);
		VGA_x++;

		if (VGA_x > VGA_WIDTH){
			VGA_x = 0;
			VGA_y++;
		}
		if (VGA_y > VGA_HEIGTH) {
			VGA_x = 0;
			VGA_y = 0;
		}
	}
	return 0;
}

uint16_t vga_entry(char character) {
	return (uint16_t) character | (uint16_t) 0xF << 8;
}
