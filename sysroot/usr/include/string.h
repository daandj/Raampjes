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
char *strtok(char *s1, const char *s2);
char *strchr(const char *s, int c);

#ifdef __cplusplus
}
#endif

#endif
