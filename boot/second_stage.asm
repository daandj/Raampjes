bits 16
org 0x7e00

jmp main

%include "boot/includes/print.asm"
%include "boot/includes/a20.asm"
%include "boot/includes/memory_map.asm"

main:
	xor ax, ax
	mov bx, ax
	mov cx, ax
	mov si, Hello
	call Print

	call enable_a20

	mov si, a20_done
	call Print

	mov si, mmap_start
	call Print

	mov di, 0x0500						; The address where the memory map will be.
	call map_memory
	jc mmap_failure

	cli
	hlt
	xchg bx, bx

mmap_failure:
	mov si, mmap_failed
	call Print

Hello: 						DB "Hello from second stage bootloader!", 10, 13, 0
a20_done:					DB "a20 line is enabled.", 10, 13, 0
mmap_start:				DB "Mapping memory...", 10, 13, 0
mmap_done:				DB "Memory is mapped.", 10, 13,0
mmap_failed: 			DB "The attempts to map the memory have failed.", 10, 13, 0
