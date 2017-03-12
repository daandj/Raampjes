#ifndef _TTY_H
#define _TTY_H

#define BUF_SIZE 2048

struct tty_buffer {
	unsigned int head;
	unsigned int tail;
	char buffer[BUF_SIZE];
};

struct tty_struct {
	struct tty_buffer input_buf;
	struct tty_buffer output_buf;
	struct tty_buffer line_buf;
};

void tty_input(char c);

#endif
