#ifndef _IS_LIBK

#include <stdlib.h>
#include <stdint.h>

typedef union header {
	struct {
		union header *next;
		int size;
	} s;
	uint64_t align;
} Header;

extern Header *free_p;

void free(void *freed) {
	Header *bp, *p;

	bp = (Header *)freed - 1;
	for (p = free_p; !(bp > p && bp < p->s.next); p = p->s.next) {
		if (p >= p->s.next && (bp > p || bp < p->s.next))
			break;
	}

	if (bp + bp->s.size == p->s.next) {
		bp->s.size += p->s.next->s.size;
		bp->s.next = p->s.next->s.next;
	} else
		bp->s.next = p->s.next;
	if (p + p->s.size == bp) {
		p->s.size += bp->s.size;
		p->s.next = bp->s.next;
	} else 
		p->s.next = bp;

	free_p = p;
}

#endif
