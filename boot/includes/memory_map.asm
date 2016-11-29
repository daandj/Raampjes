; Set ES:DI to the address of the buffer where the memory map should be placed.
map_memory:
	xor si, si									; Set up parameters.
	xor ebx, ebx
	xor eax, eax
	mov ax, 0xe820							; BIOS function.
	mov edx, 0x534d4150 				; (SMAP).
	mov cx, 24
	int 0x15
	add di, 24									; Next entry.
	inc si											; Keep a counter.
	jc Error
	mov edx, 0x534d4150
	cmp eax, edx
	jne Error
.mloop:
	mov cx, 24
	mov ax, 0xe820
	mov edx, 0x534d4150
	int 0x15
	jc MapFinished
	cmp ebx, 0
	je MapFinished
	add di, 24
	inc si
	jmp .mloop
Error:
	stc	
MapFinished:
	mov word [map_entries], si	; Save counter.
	ret

map_entries:						DW 0x0000
