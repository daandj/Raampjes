#include <string.h>
#include <stddef.h>

static char *tok_next = NULL;

char *strtok(char *s1, const char *s2) {
	char *c;
	 
	if (tok_next == NULL && s1 == NULL)
		return NULL;

	if (s1 != NULL) {
		for (c = s1; *c; c++) {
			if (!strchr(s2, *c)) {
				tok_next = c;
				break;
			}
		}
	}

	for (c = tok_next; *tok_next; tok_next++) {
		if (strchr(s2, *tok_next)) {
			*tok_next++ = '\0';
			return c;
		}
	}

	tok_next = NULL;
	return c;
}
