#include <stdint.h>
#include <stdarg.h>
#include <vga.h>
#include <stdlib.h>
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

void putchar(char c) {
	if (c == '\n') {
		VGA_x = 0;
		VGA_y++;
		return;
	}

	int index = VGA_y * VGA_WIDTH + VGA_x;
	vga_memory_address[index] = vga_entry(c);
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

int vkprintf(const char *format, va_list ap) {
	int ival;
	char *sval, *c;

	for (c = format; *c; *c++) {
		if (*c != '%') {
			putchar(*c);
			continue;
		}
		switch (*++c) {
			case 'i':
			case 'd':
				ival = va_arg(ap, int);
				char str[12];
				itoa(ival, str);
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
