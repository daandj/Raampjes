#include <raampjes/vga.h>
#include <raampjes/interrupts.h>
#include <raampjes/PIC.h>
#include <raampjes/keyboard.h>
#include <stdint.h>
#include <unistd.h>
#include <raampjes/mm.h>
#include <raampjes/sched.h>
#include <raampjes/gdt.h>
#include <raampjes/ramdisk.h>
#include <raampjes/panic.h>

char *const envp[] = { (char *)0 };
char *cmd[] = { "test", (char *)0 };

int kmain(uint32_t magic, uintptr_t addr) {
	struct multiboot_tag *tag;
	addr = VIRTUAL_ADDRESS(addr);

	init_vga();
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		panic("Bootloader not multiboot compliant: %x\n", magic);

	init_interrupts();
	keyboard_init();
	PIC_init();
	init_gdt();
	enable_interrupts();
	for (tag = (struct multiboot_tag *)(addr + 8);
			tag->type != MULTIBOOT_TAG_TYPE_END;
			tag = (struct multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
		switch (tag->type) {
			case MULTIBOOT_TAG_TYPE_MMAP:
				init_mm((struct multiboot_tag_mmap *)tag);
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				init_rd((struct multiboot_tag_module *)tag);
			default:
				break;
		}
	}

	init_sched();
	execve("bin/test", cmd, envp);
	while (1) ;
	return 0;
}
