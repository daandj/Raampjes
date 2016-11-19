Print:
	push ax
	mov ah, 0x0e
.repeat
	lodsb
	cmp al, 0
	je PrintDone
	int 0x10
	jmp .repeat
PrintDone:
	pop ax
	ret
