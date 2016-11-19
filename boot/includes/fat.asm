; Set ES:BX to the adress you want to load the FAT at.
LoadFAT:
	xor ax, ax
	inc ax
	add ax, word [ReservedSectors]
	mov cx, word [SectorsPerFAT]
	call ReadOneSector
	inc ax
	add bx, word [BytesPerSector]
	call ReadOneSector
	ret
