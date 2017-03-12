#include <unistd.h>

char hello[] = "Hello from user space!!";

int main() {
	asm ("xchg %bx, %bx");
	int a = 0;
	write(1, &hello[0], sizeof(hello) - sizeof(hello[0]));
	for (;;) ;
}
