#include <sys/wait.h>
#include <unistd.h>

char hello[] = "Hello from user space.\n";

int main() {
	int status = 0;
	pid_t idle_pid;

	if (!(idle_pid = fork())) {
		while (1) {
			pause();
		}
	}

	if (!fork())
		execve("bin/sh", NULL, NULL);

	while (1) {
		wait(&status);
	}
}
