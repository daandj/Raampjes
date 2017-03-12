#ifdef __STDC_HOSTED__

#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>

_SYSCALL3(ssize_t, read, int, fildes, void *, buf, size_t, nbytes)

#endif
