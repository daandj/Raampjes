section .text

global _start
extern _init
extern main
extern exit
_start:
	mov ebp, 0
	push ebp
	mov ebp, esp

	push esi
	push edi

	; call init_std_lib
	call _init

	pop edi
	pop esi

	call main

	mov edi, eax
	call exit
