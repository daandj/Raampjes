#ifndef _SCHED_H
#define _SCHED_H

#include <sys/types.h>
#include <stdint.h>

#define MAX_TASKS    64
#define TASK_RUNNING 1
#define TASK_STOPPED 2
#define TASK_WAITING 3

#define switch_to_userspace(x) \
	asm volatile (".intel_syntax noprefix;" \
			"mov esp, eax;" \
			"pop gs;" \
			"pop fs;" \
			"pop es;" \
			"pop ds;" \
			"popa;" \
			"add esp, 4;" \
			"iret;" \
			".att_syntax prefix" \
			:: "a" ((uint32_t)x))

struct TSS {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldtr;
	uint32_t iopb_offset;
} __attribute__((__packed__));

typedef struct stack_frame {
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t int_error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t orig_esp;
	uint32_t ss;
} __attribute__((__packed__)) StackFrame;

typedef struct task_struct {
	uint32_t esp;
	uint32_t eip;
	uint32_t ebp;
	uint32_t *page_directory;
	/* Everything above this must stay in the current order! */
	uint32_t esp0;
	pid_t pid, ppid;
	int state;
	int exit_status;
	uint32_t code_end, data_end, brk, stack_start, stack_end;
} TaskStruct;

extern TaskStruct *current;

void init_sched();
void sched();
void set_stack_frame();
int do_pause();
void wake_up(TaskStruct *p);
void wake_up_all();

#endif
