#ifndef _IS_LIBK

#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>

_SYSCALL3(ssize_t, read, int, fildes, void *, buf, size_t, nbytes)

#endif
