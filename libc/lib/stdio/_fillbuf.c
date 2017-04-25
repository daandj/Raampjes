#ifndef _IS_LIBK

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int _fillbuf(FILE *f) {
	if (f->base == NULL)
		if ((f->base = malloc(BUFSIZ)) == NULL)
			return EOF;
	f->ptr = f->base;
	f->cnt = read(f->fd, f->base, BUFSIZ);
	if (f->cnt <= 0)
		return EOF;

	return (int) *f->ptr++;
}

#endif
