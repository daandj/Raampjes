#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE_BUF 512
#define ALLOC_ERR "bc: allocation error\n"
#define BUF_ERR "bc: buffer too small\n"

char str_equals[] = " = ";

char *get_line();
int parse_line(const char *ln);
char *itoa(int n, char s[], int base);

int main(int argc, char *argv[]) {
	char *line, answer_str[20];
	int answer;

	line = get_line();
	answer = parse_line(line);
	fputs(line, stdout);
	fputs(str_equals, stdout); 
	/* We must pass a variable instead of the string
	 * literal, else gcc optimizes it with a fwrite. */
	itoa(answer, answer_str, 10);
	puts(answer_str);

	return 0;
}

char *get_line() {
	int c, index = 0;
	char *ln;

	ln = (char *)malloc(SIZE_BUF * sizeof(char));
	if (ln == NULL)
		return NULL;

	c = getchar();
	while (c != '\n' && c != EOF) {
		ln[index++] = (char)c;

		if (index >= SIZE_BUF) {
			write(stderr->fd, BUF_ERR, sizeof(BUF_ERR) - sizeof(BUF_ERR[0]));
			exit(1);
		}

		c = getchar();
	}

	ln[index] = '\0';

	return ln;
}

int parse_line(const char *ln) {
	char *line, *token, op;
	char **tokens;
	int len, index = 0, num1, num2;

	len = strlen(ln);
	line = (char *)malloc(len * sizeof(char));
	tokens = (char **)malloc(SIZE_BUF * sizeof(char *));
	if (line == NULL || tokens == NULL) {
		write(stderr->fd, ALLOC_ERR, sizeof(ALLOC_ERR) - sizeof(ALLOC_ERR[0]));
		exit(1);
	}
	memcpy(line, ln, len);

	token = strtok(line, " \t");
	while (token != NULL) {
		tokens[index++] = token;

		if (index >= SIZE_BUF) {
			write(stderr->fd, BUF_ERR, sizeof(BUF_ERR) - sizeof(BUF_ERR[0]));
			exit(1);
		}

		token = strtok(NULL, " \t");
	}

	num1 = atoi(tokens[0]);
	num2 = atoi(tokens[2]);
	if (strlen(tokens[1]) > 1) {
		puts("parse error\n");
		return 0;
	}

	op = tokens[1][0];

	switch (op) {
	case '*':
		return num1 * num2;
	case '/':
		return num1 / num2;
	case '+':
		return num1 + num2;
	case '-':
		return num1 - num2;
	default:
		puts("parse error");
	}

	return 0;
}

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
