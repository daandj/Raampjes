#include <raampjes/gdt.h>
#include <raampjes/mm.h>
#include <raampjes/sched.h>
#include <raampjes/cpu.h>
#include <raampjes/panic.h>
#include <raampjes/interrupts.h>
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
			insert_process(x); \
			switch_task(current, x); \
		} \
		enable_interrupts(); \
	} while(0)

union task_union {
	TaskStruct task;
	char stack[PAGE_SIZE];
};

int insert_process(TaskStruct *process);
int remove_process(TaskStruct *process);
TaskStruct *next_process();
void switch_tss(TaskStruct *next);
int find_empty_task();

extern uint32_t PageDirectory[];
extern struct GDT_entry GDT[];
extern void switch_task(TaskStruct *prev, TaskStruct *next);

struct TSS main_tss;
static union task_union init_union;
TaskStruct *tasks[MAX_TASKS] = {&(init_union.task), };
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
	TaskStruct *next = current->next_process;

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
	// new_task->page_directory = current->page_directory;
	int new_pid = find_empty_task();
	if (new_pid == 0)
		panic("Maximum amount of tasks reached.");
	new_task->pid = new_pid;
	tasks[new_pid] = new_task;

	wake_up_new(new_task);
	if (current->pid == new_task->pid)
		return 0;
	else
		return new_task->pid;
}

void prepare_init_task() {
	INIT_TASK->pid = 0;
	INIT_TASK->esp0 = (uintptr_t)INIT_TASK + PAGE_SIZE;
	INIT_TASK->page_directory = PageDirectory;

	INIT_TASK->next_process = INIT_TASK;
	INIT_TASK->prev_process = INIT_TASK;

	StackFrame *stack = (void *)(INIT_TASK->esp0 - sizeof(StackFrame));
	stack->gs = stack->fs = stack->es = stack->ds = stack->ss = SEG_USER_DATA;
	stack->cs = SEG_USER_CODE;
	stack->eflags = 0x202;
}

int insert_process(TaskStruct *process) {
	current->next_process->prev_process = process;
	process->next_process = current->next_process;
	current->next_process = process;
	process->prev_process = current;
	return 0;
}

int remove_process(TaskStruct *process) {
	process->prev_process->next_process = process->next_process;
	process->next_process->prev_process = process->prev_process;
	process->next_process = process->prev_process = NULL;
	return 0;
}

TaskStruct *next_process() {
	return current->next_process;
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
