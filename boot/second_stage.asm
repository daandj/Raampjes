bits 16
org 0x7e00

jmp main

%define MMAP_ADDRESS 0x0500

%include "boot/includes/print.asm"
%include "boot/includes/a20.asm"
%include "boot/includes/memory_map.asm"
%include "boot/includes/floppy.asm"
%include "boot/includes/fat.asm"
%include "boot/includes/OEM.asm"
%include "boot/includes/GDT.asm"

main:
	mov byte [DriveNumber], dl
	xor ax, ax
	mov bx, ax
	mov cx, ax
	mov si, Hello
	call Print

	call enable_a20

	mov si, a20_done
	call Print

	mov bx, 0x0500
	call LoadRootDir

	mov si, 0x0500
	mov ax, KernelFileName
	call FindDirEntry

	mov ax, word [es:di+0x0F]
	mov word [KernelCluster], ax

	mov si, 0x0500
	mov ax, RDFileName
	call FindDirEntry

	mov ax, word [es:di+0x0F]
	mov word [RDCluster], ax

	mov bx, 0x0500
	call LoadFAT

  mov bx, ELF_header
	mov word [FileAddress], bx
	mov ax, word [KernelCluster]
	call LoadFile

	mov word [RDLoadAddress], bx
	mov ax, word [RDCluster]
	call LoadFile

	sub bx, word [RDLoadAddress]
	mov word [RDSize], bx

	mov si, mmap_start
	call Print

	mov di, MMAP_ADDRESS				; The address where the memory map will be.
	call map_memory
	jc mmap_failure

	call LoadGDT

; Switch to 32 bit protected mode.
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp 0x08:PMode

	[bits 32]
PMode:
	mov ax, 0x10 ; update data segment register with new GDT entries.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	call LoadELF

; Pass the location and size of the memory map to the kernel 
; through registers SI and CX respectively.
	xor ebx, ebx
	mov ecx, ebx
	mov edx, ebx
	mov esi, ebx
	mov si, MMAP_ADDRESS
	mov cx, [map_entries]
	mov bx, word [RDLoadAddress]
	mov dx, word [RDSize]
	
	sub eax, 0xC0000000		; The entry point contains the virtual, not the physical address.
	jmp eax

	cli
	hlt

mmap_failure:
	mov si, mmap_failed
	call Print

Hello:            DB "Hello from second stage bootloader!", 10, 13, 0
a20_done:         DB "a20 line is enabled.", 10, 13, 0
mmap_start:       DB "Mapping memory...", 10, 13, 0
mmap_done:        DB "Memory is mapped.", 10, 13,0
mmap_failed:      DB "The attempts to map the memory have failed.", 10, 13, 0
not_found:        DB 'Failed', 0
KernelFileName:   DB "KERNEL  ELF"
RDFileName:       DB "INITRD  TAR"
KernelCluster:    DW 0x0000
RDCluster:        DW 0x0000
RDLoadAddress:    DW 0x0000
RDSize:           DW 0x0000

ELF:
%include "boot/includes/elf.asm"
