#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void abort(void);
int atexit(void (*)(void));
int atoi(const char*);
void free(void *ptr);
char* getenv(const char*);
void* malloc(size_t size);
void exit(int status);

#ifdef __cplusplus
}
#endif

#endif
