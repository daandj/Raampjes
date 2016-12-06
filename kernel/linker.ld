OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

VIRTUAL	 = 0xC0000000;
PHYSICAL = 0x00100000;

SECTIONS {
	. = VIRTUAL;

	.text ALIGN(0x1000) : AT(ADDR(.text) + PHYSICAL - VIRTUAL) {
		*(.bootstrap)
		*(.text)
	}

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) + PHYSICAL - VIRTUAL) {
		*(.rodata)
	}

	.data ALIGN(0x1000) : AT(ADDR(.data) + PHYSICAL - VIRTUAL) {
		*(.data)
	}

	.bss ALIGN(0x1000): AT(ADDR(.bss) + PHYSICAL - VIRTUAL) {
		*(.bss)
		*(COMMON)
	}
}
