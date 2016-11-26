bits	16							; We are still in 16 bit Real Mode
org 0x7c00

start:          jmp loader					; jump over OEM block

OEM:										DB "Raampjes"
BytesPerSector:					DW 512
SectorsPerCluster:			DB 1
ReservedSectors:				DW 1
NumberOfFATs:						DB 2
RootDirectoryEntries:		DW 224
TotalLogicalSectors:		DW 2880
MediaDiscriptor:				DB 0xF0
SectorsPerFAT:					DW 9
SectorsPerTrack:				DW 18
HeadsPerCylinder:				DW 2
HiddenSectors:					DD 0
TotalSectorsBig:				DD 0
DriveNumber:						DB 0
Unused:									DB 0
ExtBootSignature:				DB 0x29
bsSerialNumber:					DD 0xa0a1a2a3
VolumeLabel:						DB "BOOT FLOPPY"
FileSystem:							DB "FAT12   "

%include "boot/includes/print.asm"
%include "boot/includes/floppy.asm"
%include "boot/includes/fat.asm"

loader:
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp,	0x7c00 				; Set up stack

	mov si, load_root_dir
	call Print

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

	mov si, loaded
	call Print

	jmp 0x0000:0x7e00

load_fat 								db 'Loading File Allocation Table...', 10, 13, 0
load_root_dir						db 'Loading Root Directory...', 10, 13, 0
loaded 									db 'Loaded', 10, 13, 0
not_found								db 'Failed', 0
FileName								db 'STAGE2  BIN'   
Cluster									dw 0x0000

times 510 - ($ - $$) DB 0
DB 0x55
DB 0xAA
