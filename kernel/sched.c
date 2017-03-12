#include <raampjes/gdt.h>
#include <raampjes/mm.h>
#include <raampjes/sched.h>
#include <raampjes/cpu.h>
#include <raampjes/panic.h>
#include <stdint.h>

#define install_tss(x) \
	GDT[MAIN_TSS_DESC/8] = \
		CREATE_DESC((int)(x), sizeof(struct TSS), 0x40, 0x89);
#define INIT_TASK tasks[0]

extern uint32_t PageDirectory[];
extern struct GDT_entry GDT[];

struct TSS main_tss;
TaskStruct *current;
TaskStruct *tasks[MAX_TASKS];

void prepare_init_task();

void init_sched() {
	prepare_init_task();
	current = tasks[0];

	main_tss.esp0 = INIT_TASK->esp0;
	main_tss.ss0 = SEG_KERNEL_DATA;
	main_tss.iopb_offset = sizeof(struct TSS);

	install_tss(&main_tss);
	
	ltr(MAIN_TSS_DESC);
}

void prepare_init_task() {
	uintptr_t esp0 = alloc_page(KERNEL_MEMORY, PG_KERN | PG_RW) + PAGE_SIZE;
	if (esp0 < KERNEL_MEMORY)
		panic("Out of kernel memory!");

	TaskStruct *init_task = (TaskStruct *)esp0 - PAGE_SIZE;
	init_task->esp0 = esp0;
	init_task->page_directory = PageDirectory;
	INIT_TASK = init_task;

	StackFrame *stack = (void *)(esp0 - sizeof(StackFrame));
	stack->gs = stack->fs = stack->es = stack->ds = stack->ss = SEG_USER_DATA;
	stack->cs = SEG_USER_CODE;
	stack->eflags = 0x202;
}
