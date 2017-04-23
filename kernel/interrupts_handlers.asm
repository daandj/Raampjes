extern main_intr_handler

%macro handler 2
global intr%1
intr%1:
	cli
	%if %2 != 1
		push dword 0
	%endif
	pusha
	cld
	push dword %1
	call main_intr_handler
	add esp, 4
	popa
	add esp, 4
	sti
	iret 
%endmacro

handler 0, 0
handler 1, 0
handler 2, 0
handler 3, 0
handler 4, 0
handler 5, 0
handler 6, 0
handler 7, 0
handler 8, 1
handler 9, 0
handler 10, 1
handler 11, 1
handler 12, 1
handler 13, 1
handler 14, 1
handler 16, 0
handler 17, 1
handler 18, 0
handler 19, 0
handler 20, 0
handler 32, 0
handler 33, 0
handler 34, 0
handler 35, 0
handler 36, 0
handler 37, 0
handler 38, 0
handler 39, 0
handler 40, 0
handler 41, 0
handler 42, 0
handler 43, 0
handler 44, 0
handler 45, 0
handler 46, 0
handler 47, 0

extern sys_table

global syscall_handler
syscall_handler:
	cli
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	mov eax, [eax * 4 + sys_table] 
	call eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	sti
	iret
