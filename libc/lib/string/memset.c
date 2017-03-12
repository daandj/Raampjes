#include <string.h>

void *memset(char *buf, int value, size_t len) {
	char *buffer = buf;
	while (len--)
		*buffer++ = (char)value;
	return buf;
}
