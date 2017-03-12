#include <stdint.h>
#include <string.h>

#include <raampjes/elf.h>
#include <raampjes/mm.h>
#include <raampjes/panic.h>

bool is_elf_file(char *file) {
	Elf32_Ehdr *_file = (Elf32_Ehdr *)file;
	return _file->e_ident[EI_MAG0] == ELFMAG0 &&
		_file->e_ident[EI_MAG1] == ELFMAG1 &&
		_file->e_ident[EI_MAG2] == ELFMAG2 &&
		_file->e_ident[EI_MAG3] == ELFMAG3;
}

void load_elf(char *file, unsigned int file_size, StackFrame *stack_frame) {
	Elf32_Ehdr *_file = (Elf32_Ehdr *)file;
	Elf32_Phdr *pheaders = (Elf32_Phdr *)(_file->e_phoff + (uintptr_t)file);

	if (_file->e_phnum == 0)
		panic("\nELF executable file is empty.\n");

	for (int i = 0; i < _file->e_phnum; i++) {
		Elf32_Phdr header = pheaders[i];
		if (header.p_type == PT_NULL)
			continue;

		int flags = PG_USER;
		if (header.p_flags & 2)
			flags |= PG_RW;

		alloc_pages(header.p_vaddr, header.p_vaddr + header.p_memsz, flags);

		memcpy((char *)header.p_vaddr, file + header.p_offset, header.p_filesz);
		if (header.p_filesz < header.p_memsz) {
			unsigned int difference = header.p_memsz - header.p_filesz;
			memset((char *)header.p_vaddr + header.p_filesz, 0, difference);
		}
	}
	stack_frame->eip = _file->e_entry;
}
