bits 16
org 0x7e00

jmp main

%include "boot/includes/print.asm"
%include "boot/includes/a20.asm"

main:
	xor ax, ax
	mov bx, ax
	mov cx, ax
	mov si, Hello
	call Print

	call enable_a20

	cli
	hlt

Hello: 		DB "Hello from second stage bootloader!", 10, 13, 0
