[bits 32]

global _start
_start:
	mov al, 'A'
	mov ah, 3
	mov edx, 0xb8000
	mov [edx], ax
	xchg bx, bx
	
	cli
	hlt
