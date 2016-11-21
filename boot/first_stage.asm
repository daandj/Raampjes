bits	16							; We are still in 16 bit Real Mode

start:          jmp loader					; jump over OEM block

TIMES 0Bh-$+start DB 0

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
FileSystem:							DB "FAT16   "

%include "boot/includes/print.asm"
%include "boot/includes/floppy.asm"
%include "boot/includes/fat.asm"

loader:
	mov ax, 0x07c0
	mov ds, ax
	mov es, ax

	cli
	xor ax, ax
	mov ss, ax
	mov sp,	0x7c00 				; Set up stack
	sti

	mov si, hello_msg
	call Print

	mov si, load_root_dir
	call Print

	mov bx, 0x7e00
	call LoadRootDir

	mov si, 0x7e00
	call FindDirEntry

	mov bx, 0x7e00				; The address we want to load the next bootloader at
	call LoadFAT					; ( 0x7c00 + 0x200 ).

	mov si, loaded
	call Print

	xchg bx, bx
	cli
	hlt

hello_msg 							db 'Hello world!', 10, 13, 0
load_fat 								db 'Loading File Allocation Table...', 10, 13, 0
load_root_dir						db 'Loading Root Directory...', 10, 13, 0
loaded 									db 'Loaded', 10, 13, 0
FileName								db 'stage2  bin'   

times 510 - ($ - $$) DB 0
DB 0x55
DB 0xAA
