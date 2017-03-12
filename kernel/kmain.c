#include <raampjes/vga.h>
#include <raampjes/interrupts.h>
#include <raampjes/PIC.h>
#include <raampjes/keyboard.h>
#include <stdint.h>
#include <raampjes/mm.h>
#include <raampjes/sched.h>
#include <raampjes/gdt.h>
#include <raampjes/ramdisk.h>

char *const envp[] = { (char *)0 };
char *cmd[] = { "test", (char *)0 };

int kmain(struct MMap *map, 
		      uint16_t mmap_size, 
		      uint16_t RD_size, 
					char *RD_address) {
	init_vga();
	kprintf("Entered Kernel!!\n");
	init_interrupts();
	keyboard_init();
	PIC_init();
	init_gdt();
	kprintf("Not stuck on GDT\n");
	enable_interrupts();
	init_mm(map, mmap_size);
	kprintf("Or mmap\n");
	init_rd(VIRTUAL_ADDRESS(RD_address), VIRTUAL_ADDRESS(RD_size));
	kprintf("or ramdisk\n");
	init_sched();
	kprintf("or scheduler\n");
	execve("bin/test", cmd, envp);
	kprintf("Entering user space!\n");
	switch_to_userspace();
	while (1) ;
	return 0;
}
