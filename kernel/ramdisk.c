#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <raampjes/ramdisk.h>
#include <raampjes/mm.h>
#include <raampjes/panic.h>
#include <raampjes/sched.h>
#include <raampjes/elf.h>


static TarHeader *first_header;
static unsigned int tar_size;

unsigned int getsize(const char *in);
TarHeader *find_file(const char *name);
void load_file(char *file, unsigned int file_size, StackFrame *stack_frame);
void load_bin(char *file, unsigned int file_size, StackFrame *stack_frame);

void init_rd(void *address, unsigned int size) {
	first_header = (TarHeader *)address;
	tar_size = size;
}

void execve(const char *path, char *const argv[], char *const envp[]) {
	TarHeader *file_header;
	if (NULL == (file_header = find_file(path)))
		panic("\n*** PANIC ***\nFile: %s not found.", path);
	unsigned int file_size = getsize(file_header->size);

	uintptr_t page_frame = alloc_page_frame();
	uintptr_t stack = map_page(page_frame, KERNEL_MEMORY - PAGE_SIZE, 
			PG_USER | PG_RW);

	StackFrame *stack_frame = (StackFrame *)(current->esp0 - sizeof(StackFrame));

	load_file((char *)file_header + 512, file_size, stack_frame);

	current->esp = (uintptr_t)stack_frame;
	stack_frame->orig_esp = stack + PAGE_SIZE;
}

unsigned int getsize(const char *in) {
	unsigned int size = 0;
	unsigned int j;
	unsigned int count = 1;

	for (j = 11; j > 0; j--, count *=8)
		size += ((in[j - 1] - '0') * count);

	return size;
}

TarHeader *find_file(const char *name) {
	uintptr_t address = (uintptr_t)first_header;
	TarHeader *current_header = (TarHeader *)address;

	while (strncmp(name, current_header->filename, 100)) {
		unsigned int size = getsize(current_header->size);

		address += ((size - 1) / 512 + 2) * 512;

		if (current_header->filename[0] == '\0')
			return NULL;
		current_header = (TarHeader *)address;
	}

	return current_header;
}

void load_file(char *file, unsigned int file_size, StackFrame *stack_frame) {
	if (is_elf_file(file)) {
		load_elf(file, file_size, stack_frame);
	} else {
		load_bin(file, file_size, stack_frame);
	}
}

void load_bin(char *file, unsigned int file_size, StackFrame *stack_frame) {
	alloc_pages(0, file_size, PG_USER | PG_RW);

	memcpy((char *)0, file, file_size);
	stack_frame->eip = 0;
}
