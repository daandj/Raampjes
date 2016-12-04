gdt_start:
	dd 0
	dd 0
; code descriptor:
	dw 0xffff
	dw 0x0000
	db 0x0
	db 0x9a
	db 11001111b
	db 0x00
; data descriptor:
	dw 0xffff
	dw 0x0000
	db 0x0
	db 0x92
	db 11001111b
	db 0x00
gdt_end:

gdtr:
	dw gdt_end - gdt_start
	dd gdt_start

LoadGDT:
	cli
	lgdt [gdtr]
	sti
	ret
