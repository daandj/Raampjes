OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

VIRTUAL	 = 0xC0000000;
PHYSICAL = 0x00100000;

SECTIONS {
	. = VIRTUAL;
	. += PHYSICAL;

	.text ALIGN(0x1000) : AT(ADDR(.text) - VIRTUAL) {
		*(.bootstrap)
		*(.text)
	}

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) - VIRTUAL) {
		*(.rodata)
	}

	.data ALIGN(0x1000) : AT(ADDR(.data) - VIRTUAL) {
		*(.data)
	}

	.bss ALIGN(0x1000): AT(ADDR(.bss) - VIRTUAL) {
		*(.bss)
		*(COMMON)
	}
}
