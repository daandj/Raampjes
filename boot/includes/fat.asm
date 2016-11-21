; Set ES:BX to the adress you want to load the FAT at.
LoadFAT:
	xor ax, ax
	inc ax
	add ax, word [ReservedSectors]
	mov cx, word [SectorsPerFAT]
	call ReadSectors
	ret

; Set ES:BX to the adress you want to load the root directory at.
LoadRootDir:
	push ax
	push cx
	xor ax, ax
	mov ax, 0x0020										; The size of a directory entry (32bytes).
	mul word [RootDirectoryEntries]		; AX = the size of the root dir in bytes.
	div word [BytesPerSector] 				; AX = the size of the root dir in sectors.
	xchg ax, cx
	mov al, byte [NumberOfFATs]
	mul word [SectorsPerFAT]
	add ax, word [ReservedSectors]
	call ReadSectors
	pop ax
	pop cx
	ret

; Set SI to the address of the directory.
FindDirEntry:
	mov ax, word [RootDirectoryEntries]
start_loop:
	push si
	mov di, [FileName]
	mov cx, 0x000B
rep cmpsb
	pop si
	je EntryFound
	add si, 0x20
	dec ax
	jz NotFound
	jmp start_loop
EntryFound:
	ret
NotFound:
	stc
	ret
	
	

