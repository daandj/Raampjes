#include <raampjes/tty.h>
#include <raampjes/vga.h>
#include <raampjes/sched.h>
#include <stddef.h>
#include <sys/types.h>

#define FULL(buf_ptr) (buf_ptr->head == (buf_ptr->tail + 1))
#define EMPTY(buf_ptr) (buf_ptr->head == buf_ptr->tail)

typedef int ssize_t;

int buf_enqueue(struct tty_buffer *buf, char c);
char buf_dequeue(struct tty_buffer *buf);
void move_queue(struct tty_buffer *from, struct tty_buffer *to);
int buf_queue_length(struct tty_buffer *buf);

struct tty_struct terminal = {
	{0,0,{0,},},
	{0,0,{0,},},
	{0,0,{0,},},
};

void tty_input(char c) {
	switch (c) {
		case '\b':
			if (buf_dequeue(&terminal.line_buf))
				vga_putchar(c);
			break;
		case '\n':
			if (buf_enqueue(&terminal.line_buf, c))
				vga_putchar(c);
			move_queue(&terminal.line_buf, &terminal.input_buf);
			wake_up_all();
			break;
		default:
			if (buf_enqueue(&terminal.line_buf, c))
				vga_putchar(c);
	}
}

ssize_t do_write(int fildes, const void *buf, size_t nbytes) {
	unsigned int i;
	char *buffer = (char *)buf;

	switch (fildes) {
		case 0:
			for (i = 1; i < nbytes; i++)
				buf_enqueue(&terminal.input_buf, buffer[i]);
			return i + 1;
		case 1:
		case 2:
			for (i = 0; i < nbytes; i++)
				vga_putchar(buffer[i]);
			return i;
		default:
			kprintf("Writing to files is not (yet) supported...");
			return -1;
	}
}

ssize_t do_read(int fildes, void *buf, size_t nbytes) {
	unsigned int i;
	char *buffer = (char *)buf;
	size_t buf_len;

	switch (fildes) {
		case 0:
			buf_len = buf_queue_length(&terminal.input_buf);
			while (buf_len == 0) {
				do_pause();
				buf_len = buf_queue_length(&terminal.input_buf);
			}
			if (buf_len > nbytes)
				for (i = 0; i < nbytes; i++)
					buffer[i] = buf_dequeue(&terminal.input_buf);
			else
				for (i = 0; i < buf_len; i++)
					buffer[i] = buf_dequeue(&terminal.input_buf);
			return i;
		case 1:
		case 2:
			return 0;
		default:
			kprintf("Reading from files is not (yet) supported...");
			return -1;
	}

}

int buf_enqueue(struct tty_buffer *buf, char c) {
	if (FULL(buf))
		return 0;

	buf->buffer[buf->tail] = c;
	buf->tail = (buf->tail + 1) % BUF_SIZE;
	return 1;
}

char buf_dequeue(struct tty_buffer *buf) {
	if (EMPTY(buf))
		return (char)0;

	int current_head = buf->head;
	buf->head = (current_head + 1) % BUF_SIZE;

	return buf->buffer[current_head];
}

void move_queue(struct tty_buffer *from, struct tty_buffer *to) {
	int nr = buf_queue_length(from);
	while (nr--) {
		buf_enqueue(to, buf_dequeue(from));
	}
}

int buf_queue_length(struct tty_buffer *buf) {
	return (buf->tail - buf->head + BUF_SIZE) % BUF_SIZE;
}
