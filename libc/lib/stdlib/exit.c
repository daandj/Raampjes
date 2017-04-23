#ifndef _IS_LIBK

#include <stdlib.h>
#include <unistd.h>

void exit(int status) {
	_exit(status);
}

#endif
