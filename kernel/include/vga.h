#include <stdarg.h>

int kprintf(const char *format, ...);
int vkprintf(const char *format, va_list ap);
void putchar(char c);
void move_cursor(int x, int y);
int init_vga();
