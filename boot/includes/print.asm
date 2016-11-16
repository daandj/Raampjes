Print:
	lodsb
	or al, al
	jz  PrintDone
	mov ah, 0x0e
	int 10h
	jmp Print
PrintDone:
	ret
