; Set AX to the sector to convert

LBAToCHS:
	push ax
	xor dx, dx
	div word [SectorsPerTrack]
	inc dl
	mov byte [SectorNumber], dl
	xor dx, dx
	div byte [HeadsPerCylinder]
	mov byte [HeadNumber], dl
	mov byte [CylinderNumber], al
	pop ax
	ret

; Set ES:BX to the data buffer, AX to the sector you want to start to read
; and CX to the ammount of sectors.
ReadSector:
	mov di, 0x05					; Amount of retries.
	xor si, si						; Total sectors read.
NextSector:
	cmp si, cx
	je Done								; Read enough sectors.
	inc si
	push ax
	push bx
	push cx
	add ax, si						; Choose the sector to read next.
	call LBAToCHS
	mov ax, si
	mul word [BytesPerSector]
	add bx, ax
	mov ah, 0x02
	mov al, 0x01
	mov ch, byte [CylinderNumber]
	mov cl, byte [SectorNumber]
	mov dh, byte [HeadNumber]
	mov dl, byte [DriveNumber]
	int 0x13
	pop cx
	pop bx
	pop ax
	jc NextSector
	mov ah, 0x00
	int 0x13
	dec di
	jnz NextSector
Done:
	ret

SectorNumber:		DB 0
HeadNumber:			DB 0
CylinderNumber:	DB 0
