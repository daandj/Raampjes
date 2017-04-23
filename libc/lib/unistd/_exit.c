#ifndef _IS_LIBK

#include <unistd.h>

void _exit(int status) {
	asm ("int $0x30" :: "a" (_SYSI__exit), "b" (status));
}

#endif
