struc Phdr
	.p_type:					resd 1
	.p_offset:				resd 1
	.p_vaddr:					resd 1
	.p_paddr:					resd 1
	.p_filesz:				resd 1
	.p_memsz:					resd 1
	.p_flags:					resd 1
	.p_align:					resd 1
endstruc

; Loads all the elf segments in place and stores the entry point in eax.
LoadELF:
	mov ecx, 0
	mov cx, word [e_phnum]
	dec cx
.loop:
	push cx
	xor eax, eax
	mov ax, word [e_phentsize]
	mul cx
	add eax, dword [e_phoff]
	mov ecx, eax
	add ecx, ELF_header
	mov esi, dword [ecx + Phdr.p_offset]	
	add esi, ELF_header
	mov edi, dword [ecx + Phdr.p_paddr]
	mov ecx, dword [ecx + Phdr.p_filesz]
rep movsb
	pop cx
	cmp cx, 0x0
	jnz .loop
.done:
	mov eax, dword [e_entry]
	ret

SECTION .bss
ELF_header:
e_ident:						resb 16
e_type							resw 1
e_machine:					resw 1
e_version:					resd 1
e_entry:						resd 1
e_phoff:						resd 1
e_shoff:						resd 1
e_flags:						resd 1
e_ehsize:						resw 1
e_phentsize:				resw 1
e_phnum:						resw 1
e_shentsize:				resw 1
e_shnum:						resw 1
e_shstrndx:					resw 1

