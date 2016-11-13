bits	16							; We are still in 16 bit Real Mode
org		0x7c00						; We are loaded by BIOS at 0x7C00
  
start:          jmp loader					; jump over OEM block
	 
TIMES 0Bh-$+start DB 0
	 
bpbBytesPerSector:  				DW 512
bpbSectorsPerCluster: 			DB 1
bpbReservedSectors: 				DW 1
bpbNumberOfFATs: 						DB 2
bpbRootEntries: 						DW 224
bpbTotalSectors: 						DW 2880
bpbMedia: 	        				DB 0xF0
bpbSectorsPerFAT: 					DW 9
bpbSectorsPerTrack: 				DW 18
bpbHeadsPerCylinder: 				DW 2
bpbHiddenSectors:      			DD 0
bpbTotalSectorsBig:     		DD 0
bsDriveNumber: 	        		DB 0
bsUnused: 	        				DB 0
bsExtBootSignature: 				DB 0x29
bsSerialNumber:	        		DD 0xa0a1a2a3
bsVolumeLabel: 	        		DB "BOOT FLOPPY"
bsFileSystem: 	        		DB "FAT16   "
		  
Print:
	lodsb
	or al, al
	jz	PrintDone
	mov	ah,	0x0e
	int	10h
	jmp	Print
PrintDone:
	ret
																								 
loader:
	xor ax, ax
	mov ds, ax
	mov es, ax

	mov si, msg
	call Print

	cli
	hlt

msg DB "Hello world"

times 510 - ($ - $$) DB 0
DB 0x55
DB 0xAA
