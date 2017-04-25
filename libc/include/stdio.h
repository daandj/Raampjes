#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <stddef.h>

#define SEEK_SET 0
#define EOF      (-1)
#define BUFSIZ   1024
#define OPEN_MAX 10

typedef struct _iobuf { 
	int cnt; 
	char *ptr;
	char *base;
	int flag;
	int fd;
} FILE;

extern FILE _iob[OPEN_MAX];

#define stdin  (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

#ifdef __cplusplus
extern "C" {
#endif

#define getc(x)     (--(x)->cnt >= 0 \
		? (unsigned char) *(x)->ptr++ : _fillbuf(x))
#define getchar()   getc(stdin)
#define putchar(x)  putc((x), stdout)

int fclose(FILE*);
int fflush(FILE*);
FILE* fopen(const char*, const char*);
int fprintf(FILE*, const char*, ...);
size_t fread(void*, size_t, size_t, FILE*);
int fseek(FILE*, long, int);
long ftell(FILE*);
size_t fwrite(const void*, size_t, size_t, FILE*);
void setbuf(FILE*, char*);
int vfprintf(FILE*, const char*, va_list);

#ifdef __cplusplus
}
#endif

#endif
