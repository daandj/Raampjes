#ifndef _RAMDISK_H
#define _RAMDISK_H

#include <raampjes/multiboot2.h>

typedef struct TarHeader {
	char filename[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[8];
	char mtime[12];
	char chksum[8];
	char typeflag[1];
} __attribute__((__packed__)) TarHeader;

void init_rd(struct multiboot_tag_module *tag);
int do_execve(const char *path, char *const argv[], char *const envp[]);

#endif
