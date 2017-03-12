#include <stdarg.h>
#include <raampjes/panic.h>
#include <raampjes/vga.h>
#include <raampjes/cpu.h>

void panic(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vkprintf(fmt, args);
	va_end(args);
	kernel_halt();
}
