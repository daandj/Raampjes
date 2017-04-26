#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE_SIZ 256
#define MAX_TOKENS   128
#define NOT_FOUND "sh: file not found.\n"
#define ERR_EXIT do { \
			write(1, alloc_err, sizeof(alloc_err) - sizeof(alloc_err[0])); \
			_exit(1); \
	} while (0)

void show_prompt();
char *read_line();
char **split_tokens(char *line);
int exec_prog(const char *name, char *const *argv);

char alloc_err[] = "sh: allocation error.\n";

int main() {
	int i, status;
	char *line, **tokens;

	while (1) {
		show_prompt();
		line = read_line();
		tokens = split_tokens(line);
		status = exec_prog(tokens[0], tokens);
		free(tokens);
		free(line);
	}
}

void show_prompt() {
	write(1, "# ", 2);
}

char *read_line() {
	int c, index = 0;
	char *line;

	line = (char *)malloc(MAX_LINE_SIZ * sizeof(char));
	if (line == NULL) {
		ERR_EXIT;
	}

	c = getchar();
	while (c != '\n' && c != EOF) {
		line[index++] = (char)c;

		if (index >= MAX_LINE_SIZ) {
			ERR_EXIT;
		}

		c = getchar();
	}
	line[index] = '\0';

	return line;
}

char **split_tokens(char *line) {
	char **tokens, *token;
	int index = 0;

	tokens = (char **)malloc(sizeof(char *) * MAX_TOKENS);
	if (tokens == NULL) {
		ERR_EXIT;
	}

	token = strtok(line, " \t\n");
	while (token != NULL) {
		tokens[index++] = token;

		token = strtok(NULL, " \t\n");

		if (index >= MAX_TOKENS) {
			ERR_EXIT;
		}
	}
	tokens[index] = NULL;
	return tokens;
}

int exec_prog(const char *name, char *const *argv) {
	int status;
	pid_t pid, wait_pid;

	if (name == NULL)
		return -1;

	if (!strncmp("ps", name, 2)) {
		ps();
		return 0;
	}

	pid = fork();
	if (pid == 0) {
		if (execve(name, argv, NULL) == -1) {
			write(1, NOT_FOUND, sizeof(NOT_FOUND) - sizeof(NOT_FOUND[0]));
		}
		_exit(-1);
	} else if (pid < 0) {
		ERR_EXIT;
	} else {
		do {
			wait_pid = wait(&status);
		} while (wait_pid != pid);
		return status;
	}

	return -1;
}
