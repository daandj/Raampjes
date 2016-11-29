enable_a20:
	call ps2_writable
	mov al, 0xad							; PS/2 controller instruction to disable keyboard.
	out 0x64, al							; Send instruction to PS/2 controller.

	call ps2_writable
	mov al, 0xd0							; PS/2 controller instruction to read controller output.
	out 0x64, al

	call ps2_readable
	in al, 0x60								; Read controller output register.
	push ax

	call ps2_writable
	mov al, 0xd1							; PS/2 controller instruction to write controller output.
	out 0x64, al

	call ps2_writable
	pop ax
	or al, 2									; Enable a20 line.
	out 0x60, al							; Update controller output register on PS/2 controller

	call ps2_writable
	mov al, 0xae 							; PS/2 controller instruction to re-enable keyboard.
	out 0x64, al

	call ps2_writable 				; Wait for PS/2 controller to be done before continuing.
	ret

ps2_readable:
	in al, 0x64
	test al, 1
	jz ps2_readable
	ret

ps2_writable
	in al, 0x64
	test al, 2
	jnz ps2_writable
	ret
