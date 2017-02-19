; Set ES:BX to the adress you want to load the FAT at.
LoadFAT:
	xor ax, ax
	mov ax, word [ReservedSectors]
	mov cx, word [SectorsPerFAT]
	call ReadSectors
	ret

; Set AX to the cluster, AX will be set to the sector on return.
AddressFromCluster:
	push cx
	sub ax, 0x0002
	xor cx, cx
	mov cl, byte [SectorsPerCluster]
	mul cx
	add ax, word [DataSector]
	pop cx
	ret

LoadFile:
	mov word [Cluster], ax                  ; save the cluster
.loop:
	mov ax, word [Cluster]
	call AddressFromCluster                 ; convert cluster to LBA
	xor cx, cx
	mov cl, byte [SectorsPerCluster]        ; sectors to read
	mov bx, word [FileAddress]
	call ReadSectors
	mov ax, word [Cluster]                  ; identify current cluster
	mov cx, ax                              ; copy current cluster
	mov dx, ax                              ; copy current cluster
	shr dx, 0x0001                          ; divide by two
	add cx, dx                              ; sum for (3/2)
	mov si, 0x0500
	add si, cx                              ; index into FAT
	mov dx, word [es:si]                    ; read two bytes from FAT
	test ax, 0x0001
	jnz .odd_cluster
.even_cluster:
	and dx, 0000111111111111b               ; take low twelve bits
	jmp .done
.odd_cluster:
	shr dx, 0x0004                          ; take high twelve bits
.done:
	mov word [Cluster], dx                  ; store new cluster
	mov bx, word [FileAddress]
	add bx, 0x0200
	mov word [FileAddress], bx
	cmp dx, 0x0FF0                          ; test for end of file
	jb .loop
	ret

; Set ES:BX to the adress you want to load the root directory at.
LoadRootDir:
	pusha
	mov ax, 0x0020										; The size of a directory entry (32bytes).
	mul word [RootDirectoryEntries]		; AX = the size of the root dir in bytes.
	div word [BytesPerSector] 				; AX = the size of the root dir in sectors.
	xchg ax, cx
	xor ax, ax
	mov al, byte [NumberOfFATs]
	mul word [SectorsPerFAT]
	add ax, word [ReservedSectors]
	mov word [DataSector], ax
	add word [DataSector], cx
	call ReadSectors
	popa
	ret

; Root Directory is loaded at 0000:7e00, sets DI to the address of the right
; Directory entry.
FindDirEntry:
	mov word [FileName], ax
	mov cx, word [RootDirectoryEntries]
	mov di, si
	xor ax, ax

.loop:
	push cx
	push si
	mov cx, 11
	mov si, word [FileName]

repe cmpsb
	pop si
	je EntryFound

	add ax, 32
	mov di, si
	add di, ax

	pop cx
	loop .loop

NotFound:
	mov si, not_found
	call Print
	hlt

EntryFound:
	pop cx
	ret 

DataSector:							DW 0x0000
FileAddress:						DW 0x0000
Cluster:                DW 0x0000
FileName:               DW 0x0000
