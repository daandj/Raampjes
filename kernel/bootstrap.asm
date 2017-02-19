%define phys_addr(x) ((x)-0xc0000000)

[bits 32]
section .bss
align 0x1000
global PageDirectory
global PageTable1
PageDirectory:
	resb 4096
PageTable1:
	resb 4096
StackBegin:
	resb 16384
StackEnd:
Mmap_pointer:   resd 1
Mmap_size:      resd 1
RDAddress:			resd 1
RDSize:         resd 1

section .text
global _start
extern kmain
_start:
	mov [phys_addr(Mmap_pointer)], esi
	mov [phys_addr(Mmap_size)], cx	
	mov [phys_addr(RDAddress)], bx
	mov [phys_addr(RDSize)], dx

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

; Set up all of paging again, this time with virtual addresses.
	mov eax, phys_addr(PageDirectory)
	mov ebx, phys_addr(PageTable1)
	or ebx, 3
	mov dword [eax], ebx
	mov dword [768 * 4 + eax], ebx

; Set up stack
	mov esp, StackEnd

; Pass the kmain() parameters
	push dword [phys_addr(RDAddress)]
	push dword [phys_addr(RDSize)]
	push dword [phys_addr(Mmap_size)]
	push dword [phys_addr(Mmap_pointer)]

	mov eax, kmain    ; This is necessary to force the use of an absolute
	call eax          ; function call, this way we jump to the higher half.
	xchg bx, bx

	cli
	hlt
