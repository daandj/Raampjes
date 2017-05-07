#include <stdio.h>
#include <string.h>
#include <unistd.h>

int fputs(const char *s, FILE *stream) {
	int len = strlen(s);
	write(stream->fd, s, len);
}
