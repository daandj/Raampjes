#include <string.h>

char *strchr(const char *s, int c) {
	int len;
	unsigned char uc = (unsigned char)c;

	len = strlen(s) + 1;

	while (len--)
		if (uc == *s++)
			return (char *)s;

	return NULL;
}
