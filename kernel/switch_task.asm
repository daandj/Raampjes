global switch_task
extern current
extern switch_tss
extern page_frame

switch_task:
	push ebp
	mov ebp, esp
	push edi
	push esi
	push ebx
	mov esi, [ebp + 8]
	mov edi, [ebp + 12]

	mov [current], edi

	push edi
	call switch_tss
	add esp, 4

	push dword [edi + 12]
	call page_frame
	add esp, 4
	mov cr3, eax

	mov [esi], esp
	mov esp, [edi]
	mov [esi + 8], ebp
	mov ebp, [edi + 8]

	mov dword [esi + 4], .1
	push dword [edi + 4]
	ret
.1:
	pop ebx
	pop esi
	pop edi
	leave
	ret
