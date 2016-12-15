#include <stdlib.h>

char *itoa(int n, char s[]) {
	char const digit[] = "0123456789";
	int i, sign;
	char *p = s;

	if ((sign = n) < 0) {
		n = -n;
		*p++ = '-';
	}

	i = n;
	do {
		p++;
		i /= 10;
	} while (i > 0);
	*p = '\0';

	do {
		*--p = digit[n%10];
		n /= 10;
	} while (n > 0);
	return s;
}
