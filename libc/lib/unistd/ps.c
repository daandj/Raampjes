#include <unistd.h>

void ps() {
	int ret;
	asm ("int $0x30"
		: "=a" (ret)
		: "a" (_SYSI_ps));
	return;
}
