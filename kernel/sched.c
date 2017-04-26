#include <raampjes/gdt.h>
#include <raampjes/mm.h>
#include <raampjes/sched.h>
#include <raampjes/cpu.h>
#include <raampjes/panic.h>
#include <raampjes/interrupts.h>
#include <raampjes/vga.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#define install_tss(x) \
	GDT[MAIN_TSS_DESC/8] = \
		CREATE_DESC((int)(x), sizeof(struct TSS), 0x40, 0x89);
#define INIT_TASK tasks[0]
#define wake_up_new(x) \
	do { \
		uint32_t esp, ebp; \
		asm("movl %%esp, %[new_esp];" \
				"movl %%ebp, %[new_ebp];" \
				"movl $1f, %[new_eip];" \
				"1:;" \
				: [new_esp] "=&m" (esp), \
					[new_ebp] "=&m" (ebp), \
				  [new_eip] "=&m" (x->eip) \
				: : "ecx", "edx", "edi", "esi", "memory" \
			 ); \
		if (x != current) { \
			x->esp = (esp - (uintptr_t)current) + (uintptr_t)x; \
			x->ebp = (ebp - (uintptr_t)current) + (uintptr_t)x; \
			switch_task(current, x); \
		} \
	} while(0)

union task_union {
	TaskStruct task;
	char stack[PAGE_SIZE];
};

TaskStruct *next_process();
void switch_tss(TaskStruct *next);
int find_empty_task();
int free_task();

extern uint32_t PageDirectory[];
extern struct GDT_entry GDT[];
extern void switch_task(TaskStruct *prev, TaskStruct *next);

struct TSS main_tss;
static union task_union init_union;
TaskStruct *tasks[MAX_TASKS] = {&(init_union.task), };
TaskStruct *queue[MAX_TASKS];
int head = 0, tail = 0;
TaskStruct *current = &(init_union.task);

void prepare_init_task();

void init_sched() {
	prepare_init_task();

	main_tss.esp0 = INIT_TASK->esp0;
	main_tss.ss0 = SEG_KERNEL_DATA;
	main_tss.iopb_offset = sizeof(struct TSS);

	install_tss(&main_tss);

	ltr(MAIN_TSS_DESC);
	set_interrupt_callback(32, sched);
}

void sched() {
	TaskStruct *next = next_process();

	switch_task(current, next);
}

pid_t do_fork() {
	TaskStruct *new_task = (TaskStruct *)alloc_page(KERNEL_MEMORY,
                                                  PG_KERN | PG_RW);
	if (new_task < KERNEL_MEMORY)
		panic("Out of kernel memory.");

	memcpy(new_task, current, PAGE_SIZE);
	new_task->esp0 = (uintptr_t)new_task + PAGE_SIZE;
	new_task->page_directory = fork_mem(current->page_directory);
	int new_pid = find_empty_task();
	if (new_pid == 0)
		panic("Maximum amount of tasks reached.");
	new_task->pid = new_pid;
	new_task->ppid = current->pid;
	tasks[new_pid] = new_task;

	wake_up_new(new_task);
	if (current->pid == new_task->pid) {
		return 0;
	} else {
		return new_task->pid;
	}
}

int do_pause() {
	current->state = TASK_WAITING;
	sched();
	return -1;
}

void wake_up(TaskStruct *p) {
	if (p->state == TASK_RUNNING)
		panic("Trying to wake a running task");

	p->state = TASK_RUNNING;
	return;
}

void wake_up_all() {
	int i;

	for (i = 0; i < MAX_TASKS; i++)
		if (tasks[i] != NULL && tasks[i]->state == TASK_WAITING)
			wake_up(tasks[i]);
}

void do_exit(int status) {
	if (current->pid == 0)
		panic("*** PANIC ***\nInit trying to exit.");
	if (current->pid == 1)
		panic("*** PANIC ***\nIdle task trying to exit.");
	current->exit_status = status;
	current->state = TASK_STOPPED;

	free_mem_usr();

	if (tasks[current->ppid]->state == TASK_WAITING) {
		wake_up(tasks[current->ppid]);
	}

	sched();
}

pid_t do_wait(int *stat_loc) {
	int i;
	pid_t child_found = 0;
	while (1) {
		for (i = 0; i < MAX_TASKS; i++) {
			if (tasks[i]->ppid == current->pid &&
					tasks[i]->state == TASK_STOPPED) {
				if (stat_loc)
					*stat_loc = tasks[i]->exit_status;
				child_found = tasks[i]->pid;
				free_task(tasks[i]);
			}
		}
		if (child_found)
			return child_found;
		else
			do_pause();
	}
}

void *do_sbrk(intptr_t incr) {
	uintptr_t prev_brk = current->brk;
	if (prev_brk + incr > current->code_end
			&& prev_brk + incr < current->stack_start) {
		current->brk += incr;
		return (void *)prev_brk;
	}
	return (void *)-1;
}

void prepare_init_task() {
	INIT_TASK->pid = 0;
	INIT_TASK->state = TASK_RUNNING;
	INIT_TASK->esp0 = (uintptr_t)INIT_TASK + PAGE_SIZE;
	INIT_TASK->page_directory = PageDirectory;
}

TaskStruct *next_process() {
	int next, prev, i;
	TaskStruct *t;
	prev = current->pid + 1;
	next = 1;

	for (i = 0; i < MAX_TASKS; i++) {
		t = tasks[(i + prev) % MAX_TASKS];
		if (t && t->state == TASK_RUNNING) {
			next = t->pid;
			break;
		}
	}

	return tasks[next];
}

void switch_tss(TaskStruct *next) {
	main_tss.esp0 = next->esp0;
}

int find_empty_task() {
	for (int i = 0; i < MAX_TASKS; i++)
		if (tasks[i] == NULL)
			return i;
	return 0;
}

int free_task(TaskStruct *p) {
	tasks[p->pid] = NULL;
	free_page((uintptr_t)p->page_directory);
	free_page((uintptr_t)p);
	return 0;
}

void pretty_print(TaskStruct *t) {
	if (t == NULL)
		kprintf(" PID PPID STATE ADDR\n");
	else
		kprintf("%d %d %d %x\n", t->pid, t->ppid, t->state, t);
}

void do_ps() {
	int i;

	pretty_print(NULL);

	for (i = 0; i < MAX_TASKS; i++)
		if (tasks[i])
			pretty_print(tasks[i]);
}
