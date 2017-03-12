#include <string.h>

int strncmp(const char *s1, const char *s2, size_t n) {
	const unsigned char *str1 = (unsigned char *)s1;
	const unsigned char *str2 = (unsigned char *)s2;

	for (unsigned int i = 0; i < n; i++) {
		if (str1[i] < str2[i])
			return -1;
		else if (str1[i] > str2[i])
			return 1;
		else if (str1[i] == 0)
			return 0;
	}
	return 0;
}
