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

ResetFloppy:
	xor ax, ax
	int 0x13
	jc ResetFloppy
	ret

; Set AX to the sector you want to load, CX to the amount of sectors
; and ES:BX to the address you want to load it at.
ReadSectors:
	mov di, 0x05
ReadLoop:
	push ax
	push cx
	call LBAToCHS
	mov ah, 0x02
	mov al, 0x01
	mov ch, byte [CylinderNumber]
	mov cl, byte [SectorNumber]
	mov dh, byte [HeadNumber]
	mov dl, byte [DriveNumber]
	int 0x13
	jc Error
	add bx, 0x0200
	pop cx
	dec cx
	jcxz finished
	pop ax
	jmp ReadLoop
Error:
	call ResetFloppy
	pop cx
	dec di
	pop ax
	jnz ReadLoop
finished:
	pop ax
	ret

SectorNumber:		DB 0
HeadNumber:			DB 0
CylinderNumber:	DB 0
