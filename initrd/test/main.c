#include <unistd.h>

char hello[] = "Hello from user space!!\n";
char hello_1[] = "From parent process!\n";
char hello_2[] = "From child process!\n";

int main() {
	int a = 0;
	write(1, &hello[0], sizeof(hello) - sizeof(hello[0]));
	if (fork()) {
		write(1, &hello_1[0], sizeof(hello_1) - sizeof(hello_1[0]));
	} else {
		write(1, &hello_2[0], sizeof(hello_2) - sizeof(hello_2[0]));
	}
	for (;;) ;
}
