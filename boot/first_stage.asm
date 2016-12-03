bits	16							; We are still in 16 bit Real Mode
org 0x7c00

start:          jmp loader					; jump over OEM block

%include "boot/includes/OEM.asm"
%include "boot/includes/print.asm"
%include "boot/includes/floppy.asm"
%include "boot/includes/fat.asm"

loader:
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp,	0x7c00 				; Set up stack
	mov byte [DriveNumber], dl

	mov bx, 0x7e00
	call LoadRootDir

	mov si, 0x7e00
	call FindDirEntry

	mov ax, word [es:di+0x0F]
	mov word [Cluster], ax
	mov bx, 0x0500
	call LoadFAT
	
	mov bx, 0x7e00
	mov word [FileAddress], bx
	mov si, 0x0500
	call LoadFile

	mov dl, byte [DriveNumber]
	jmp 0x0000:0x7e00

not_found								db 'Failed', 0
FileName								db 'STAGE2  BIN'   
Cluster									dw 0x0000

times 510 - ($ - $$) DB 0
DB 0x55
DB 0xAA
