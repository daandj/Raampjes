#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *, const void *, size_t);
void *memset(char *, int, size_t);
char *strcpy(char *, const char *);
int strncmp(const char *, const char *, size_t);
size_t strlen(const char *);

#ifdef __cplusplus
}
#endif

#endif
