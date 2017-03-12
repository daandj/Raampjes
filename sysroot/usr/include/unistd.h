/* unistd.h: */
#ifndef _UNISTD_H
#define _UNISTD_H

#define _SYSI_fork            0
#define _SYSI_write           1
#define _SYSI_read            2

#define _SYSCALL0(type, name) \
	type name() { \
		type ret; \
		asm ("int $0x30" \
				: "=a" (ret) \
				: "a" (_SYSI_##name)); \
		return ret; \
	}

#define _SYSCALL3(type, name, atype, a, btype, b, ctype, c) \
	type name(atype a, btype b, ctype c) { \
		type ret; \
		asm ("int $0x30" \
				: "=a" (ret) \
				: "a" (_SYSI_##name), \
				  "b" (a), "c" (b), "d" (c)); \
		return ret; \
	}

#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int execv(const char*, char* const[]);
int execve(const char*, char* const[], char* const[]);
int execvp(const char*, char* const[]);
pid_t fork(void);
ssize_t write(int fildes, const void *buf, size_t nbytes);
ssize_t read(int fildes, void *buf, size_t nbytes);

#ifdef __cplusplus
}
#endif

#endif
