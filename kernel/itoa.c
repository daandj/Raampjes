#include <raampjes/stdlib.h>

char *itoa(int n, char s[], int base) {
	char const digit[] = "0123456789ABCDEF";
	int i, sign;
	char *p = s;

	if ((sign = n) < 0) {
		n = -n;
		*p++ = '-';
	}

	if (base == 16) {
		*p++ = '0';
		*p++ = 'x';
	}

	i = n;
	do {
		p++;
		i /= base;
	} while (i > 0);
	*p = '\0';

	do {
		*--p = digit[n%base];
		n /= base;
	} while (n > 0);
	return s;
}

char *uitoa(unsigned int n, char s[], int base) {
	char const digit[] = "0123456789ABCDEF";
	unsigned int i;
	char *p = s;

	if (base == 16) {
		*p++ = '0';
		*p++ = 'x';
	}

	i = n;
	do {
		p++;
		i /= base;
	} while (i > 0);
	*p = '\0';

	do {
		*--p = digit[n%base];
		n /= base;
	} while (n > 0);
	return s;
}
