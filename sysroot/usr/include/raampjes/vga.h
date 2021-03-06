#ifndef _VGA_H
#define _VGA_H

#include <stdarg.h>

int kprintf(const char *format, ...);
int vkprintf(const char *format, va_list ap);
void vga_putchar(char c);
void move_cursor(int x, int y);
void clr_screen();
int init_vga();

#endif
