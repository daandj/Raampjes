#include <unistd.h>

#ifdef _IS_LIBK

int execve(const char *path, char *const argv[], char *const envp[]) {
	return do_execve(path, argv, envp);
}

#else

_SYSCALL3(int, execve, const char *, path, char *const*, argv, char *const*, envp)

#endif
