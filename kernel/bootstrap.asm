%define phys_addr(x) ((x)-0xc0000000)

[bits 32]
section .bss
align 0x1000
global PageDirectory
PageDirectory:
	resb 4096
PageTable1:
	resb 4096
StackBegin:
	resb 4096
StackEnd:
Mmap_pointer:   resd 1
Mmap_size:      resw 1

section .text
global _start
extern kmain
_start:
	mov [phys_addr(Mmap_pointer)], esi
	mov [phys_addr(Mmap_size)], cx	

init_page_table:
	mov edi, phys_addr(PageTable1)
	mov ecx, 1024
.loop:
	dec cx
	mov eax, 4096
	mul ecx
	or eax, 3
	mov dword [ecx * 4 + edi], eax
	cmp ecx, 0
	jnz .loop
init_page_dir:
	mov edi, phys_addr(PageDirectory)
	mov ecx, 1023
.loop:
	mov dword [ecx * 4 + edi], 0
	loop .loop
	mov eax, phys_addr(PageTable1)
	or eax, 3
	mov dword [edi], eax
	mov dword [768 * 4 + edi], eax

enable_paging:
	mov eax, phys_addr(PageDirectory)
	mov cr3, eax

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

; Set up stack
	mov esp, StackEnd

; Pass the kmain() parameters
	push word [phys_addr(Mmap_size)]
	push dword [phys_addr(Mmap_pointer)]

	call kmain
	xchg bx, bx

	cli
	hlt
