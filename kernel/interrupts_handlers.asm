extern main_intr_handler

%macro handler 1
global intr%1
intr%1:
	xchg bx, bx
	pusha
	cld
	push byte %1
	call main_intr_handler
	popa
	iret
%endmacro

handler 0
handler 1
handler 2
handler 3
handler 4
handler 5
handler 6
handler 7
handler 8
handler 9
handler 10
handler 11
handler 12
handler 13
handler 14
handler 16
handler 17
handler 18
handler 19
handler 20
handler 32
handler 33
handler 34
handler 35
handler 36
handler 37
handler 38
handler 39
handler 40
handler 41
handler 42
handler 43
handler 44
handler 45
handler 46
handler 47
