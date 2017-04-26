#include <stdint.h>
#include <unistd.h>

typedef int (*fn_ptr)();

extern int do_fork();
extern int do_write();
extern int do_read();
extern int do_execve();
extern int do_exit();
extern int do_wait();
extern int do_pause();
extern int do_sbrk();
extern int do_ps();

fn_ptr sys_table[] = { do_fork, do_write, do_read, do_execve, 
	do_exit, do_wait, do_pause, do_sbrk, do_ps, };
