#ifndef _ELF_H
#define _ELF_H

#include <stdbool.h>
#include <stdint.h>
#include <raampjes/sched.h>

#define Elf32_Addr    uint32_t
#define Elf32_Half    uint16_t
#define Elf32_Off     uint32_t
#define Elf32_Sword   uint32_t
#define Elf32_Word    uint32_t

enum { EI_MAG0 = 0, EI_MAG1, EI_MAG2, EI_MAG3,
	     EI_CLASS, EI_DATA, EI_VERSION, EI_PAD };

enum { PT_NULL = 0, PT_LOAD, PT_DYNAMIC, 
	     PT_INTERP, PT_NOTE, PT_SHLIB, PT_PHDR };

#define ELFMAG0       0x7f
#define ELFMAG1       'E'
#define ELFMAG2       'L'
#define ELFMAG3       'F'

#define EI_NIDENT 16

typedef struct {
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half    e_type;
	Elf32_Half    e_machine;
	Elf32_Word    e_version;
	Elf32_Addr    e_entry;
	Elf32_Off     e_phoff;
	Elf32_Off     e_shoff;
	Elf32_Word    e_flags;
	Elf32_Half    e_ehsize;
	Elf32_Half    e_phentsize;
	Elf32_Half    e_phnum;
	Elf32_Half    e_shentsize;
	Elf32_Half    e_shnum;
	Elf32_Half    e_shstrndx;
} Elf32_Ehdr;

typedef struct {
	Elf32_Word    p_type;
	Elf32_Off     p_offset;
	Elf32_Addr    p_vaddr;
	Elf32_Addr    p_paddr;
	Elf32_Word    p_filesz;
	Elf32_Word    p_memsz;
	Elf32_Word    p_flags;
	Elf32_Word    p_align;
} Elf32_Phdr;

bool is_elf_file(char *file);
void load_elf(char *file, unsigned int file_size, StackFrame *stack_frame);

#endif
