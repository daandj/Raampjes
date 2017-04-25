#ifndef _IS_LIBK

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define ALLOC_SIZE 512

typedef union header {
	struct {
		union header *next;
		unsigned size;
	} s;
	uint64_t align;
} Header;

static Header *free_p = NULL;
static Header base;

Header *split_block(Header *p, unsigned size);
Header *alloc_free(unsigned size);

void *malloc(size_t size) {
	Header *p, *prev;
	unsigned block_size;

	if (size == 0)
		return NULL;
	
	block_size = (size - 1) / sizeof(Header) + 2;
	if ((prev = free_p) == NULL) {
		base.s.next = free_p = prev = &base; 
		base.s.size = 0;
	}

	for (p = prev->s.next; ; prev = p, p = p->s.next) {
		if (p->s.size == block_size) {
			prev->s.next = p->s.next;
			break;
		} else {
			p = split_block(p, block_size);
			break;
		}

		if (p == free_p)
			if ((p = alloc_free(size)) == NULL)
				return NULL;
	}

	return (Header *)(p + 1);
}

Header *split_block(Header *p, unsigned size) {
	p->s.size -= size;
	p += size;
	p->s.size = size;
	return p;
}

Header *alloc_free(unsigned size) {
	Header *p;

	if (size < ALLOC_SIZE)
		size = ALLOC_SIZE;

	p = (Header *)sbrk(size * sizeof(Header));
	if ((char *)p == (char *)-1)
		return NULL;
	p->s.size = size;
	free((void *)(p+1));
	return free_p;
}

#endif
