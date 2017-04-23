#ifndef _IS_LIBK

#include <unistd.h>
#include <sys/wait.h>

_SYSCALL1(pid_t, wait, int *, stat_loc)

#endif
