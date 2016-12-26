#include <stdarg.h>
#include <panic.h>
#include <vga.h>
#include <cpu.h>

void panic(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vkprintf(fmt, args);
	va_end(args);
	kernel_halt();
}
