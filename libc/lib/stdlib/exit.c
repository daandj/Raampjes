#include <stdlib.h>
#include <raampjes/panic.h>

void exit(int status) {
#ifdef __STDC_HOSTED__
	for (;;) ;
#else
	panic("*** PANIC ***\nFunction exit() is not intended for kernel use.");
#endif
}
