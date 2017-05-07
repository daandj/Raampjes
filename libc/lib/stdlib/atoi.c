#include <stdlib.h>
#include <string.h>

int atoi(const char *str) {
	int is_neg, i = 0, ret = 0, len = strlen(str);

	if (is_neg = (str[0] == '-'))
		i++;

	for (; i < len; i++) {
		if (str[i] >= '0' && str[i] <= '9')
			ret = ret * 10 + (str[i] - '0');
		else 
			return 0;
	}

	return is_neg ? ret * -1 : ret;
}
