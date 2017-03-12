#include <stdint.h>
#include <unistd.h>

typedef int (*fn_ptr)();

extern int do_fork();
extern int do_write();

fn_ptr sys_table [] = { do_fork, do_write, };
