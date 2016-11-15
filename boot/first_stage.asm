bits	16							; We are still in 16 bit Real Mode
org		0x7c00						; We are loaded by BIOS at 0x7C00

start:          jmp loader					; jump over OEM block

TIMES 0Bh-$+start DB 0

BytesPerSector:					DW 512
SectorsPerCluster:			DB 1
ReservedSectors:				DW 1
NumberOfFATs:						DB 2
RootDirectoryEntries:		DW 512
TotalLogicalSectors:		DW 2880
MediaDiscriptor:				DB 0xF0
SectorsPerFAT:					DW 9
SectorsPerTrack:				DW 12
HeadsPerCylinder:				DW 2
HiddenSectors:					DD 0
TotalSectorsBig:				DD 0
DriveNumber:						DB 0
Unused:									DB 0
ExtBootSignature:				DB 0x29
bsSerialNumber:					DD 0xa0a1a2a3
VolumeLabel:						DB "BOOT FLOPPY"
FileSystem:							DB "FAT16   "

%include "includes/print.asm"
%include "includes/floppy.asm"

loader:
	xor ax, ax
	mov ds, ax
	mov es, ax

	mov si, msg
	call Print

	mov bx, 0x0200
	mov ax, 2
	mov cx, 1
	call ReadSector

	cli
	hlt

msg DB "Hello world"

times 510 - ($ - $$) DB 0
DB 0x55
DB 0xAA
