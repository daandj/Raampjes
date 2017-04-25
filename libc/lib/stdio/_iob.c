#ifndef _IS_LIBK

#include <stdio.h>

FILE _iob[OPEN_MAX] = {
	{ 0, NULL, NULL, 0 /* Flags are not used yet... */, 0 },
	{ 0, NULL, NULL, 0, 1 },
	{ 0, NULL, NULL, 0, 2 }
};

#endif
