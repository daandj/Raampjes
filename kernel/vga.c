#include <stdint.h>
#include <stdarg.h>
#include <vga.h>
#include <stdlib.h>
#include <cpu.h>
#define VGA_WIDTH 80
#define VGA_HEIGTH 25

uint16_t vga_entry(char);

int VGA_x, VGA_y, cursor_x, cursor_y;
uint16_t *vga_memory_address;

int init_vga() {
	VGA_x = 0;
	VGA_y = 0;
	cursor_x = 0;
	cursor_y = 0;
	vga_memory_address = (uint16_t *) 0xb8000;
	clr_screen();
	return 0;
}

void clr_screen() {
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGTH; i++)
		vga_memory_address[i] = vga_entry(' ');
}

void putchar(char c) {
	int index;

	switch (c) {
		case '\n': 
			VGA_x = 0;
			VGA_y++;
			break;
		case '\b':
			if (VGA_x == 0) {
				VGA_x = VGA_WIDTH;

				if (VGA_y == 0) {
					VGA_y = VGA_HEIGTH;
				} else {
					VGA_y--;
				}

			} else {
				VGA_x--;
			}
			index = VGA_y * VGA_WIDTH + VGA_x;
			vga_memory_address[index] = vga_entry(' ');
			break;
		case '\t':
			kprintf("%s", (VGA_x%2 == 0) ? "  " : " ");
			break;
		default:
			index = VGA_y * VGA_WIDTH + VGA_x;
			vga_memory_address[index] = vga_entry(c);
			VGA_x++;
			break;
	}

	if (VGA_x >= VGA_WIDTH){
		VGA_x = 0;
		VGA_y++;
	}
	if (VGA_y >= VGA_HEIGTH) {
		VGA_x = 0;
		VGA_y = 0;
		clr_screen();
	}
	move_cursor(VGA_x, VGA_y);
}

int vkprintf(const char *format, va_list ap) {
	int ival;
	char *sval, *c;
	char str[12];

	for (c = format; *c; *c++) {
		if (*c != '%') {
			putchar(*c);
			continue;
		}
		switch (*++c) {
			case 'i':
			case 'd':
				ival = va_arg(ap, int);
				itoa(ival, str, 10);
				kprintf(str);
				break;
			case 'u':
				ival = va_arg(ap, int);
				uitoa(ival, str, 10);
				kprintf(str);
				break;
			case 'x':
			case 'X':
				ival = va_arg(ap, int);
				uitoa(ival, str, 16);
				kprintf(str);
				break;
			case 's':
				for (sval = va_arg(ap, char *); *sval; sval++) 
					putchar(*sval);
				break;
			case '%':
				putchar('%');
				break;
		}
	}
	return 0;
}

int kprintf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vkprintf(format, args);
	va_end(args);
	return 0;
}

uint16_t vga_entry(char character) {
	return (uint16_t) character | (uint16_t) 0xF << 8;
}

void move_cursor(int x, int y) {
	int index = y * VGA_WIDTH + x;
	outb(0x3d4, 14);
	outb(0x3d5, index >> 8);
	outb(0x3d4, 15);
	outb(0x3d5, index);
}
