#include <keyboard.h>
#include <cpu.h>
#include <vga.h>
#include <stdbool.h>
#include <stdint.h>
#include <panic.h>
#include <interrupts.h>

#define PS2_DATA					0x60
#define KEYBOARD_COMMAND	0x60
#define PS2_COMMAND				0x64

struct {
	bool shift, control, alt, caps, numlock;
} State;

const char key[] = {
	'\0', '\0', '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=', '\b', '\t',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '[', ']', '\n', '\0', 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', '\0', '*',
	'\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', '\0', '\0', '\0', '\0',
};

const char key_shift[] = {
	'\0', '\0', '!', '@', '#', '$', '%', '^',
	'&', '*', '(', ')', '_', '+', '\b', '\t',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
	'O', 'P', '{', '}', '\n', '\0', 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
	'"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', '\0', '*',
	'\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', '\0', '\0', '\0', '\0',
};

uint8_t keyboard_get_output() {
	return inb(PS2_DATA);
}

void PS2_wait_writable() {
	while (inb(PS2_COMMAND)&2) ;
}

void PS2_wait_readable() {
	uint8_t input;
	while (((input = inb(PS2_COMMAND))&1) == 0) ;
}

uint8_t PS2_get_scancode_set() {
	uint8_t input;

	PS2_wait_writable();
	outb(KEYBOARD_COMMAND, 0xf0);

	PS2_wait_writable();

	outb(KEYBOARD_COMMAND, 0x00);

	PS2_wait_readable();
	/* The ACK output (0xfa) from the keyboard controller should be ignored. */
	while ((input = inb(KEYBOARD_COMMAND)) == 0xfa) ;

	if (input == 0xfe)
		return PS2_get_scancode_set();
	else
		return input;
}

void keyboard_init() {
	disable_keyboard();
	/* int scancode_set = PS2_get_scancode_set();
	if (scancode_set != 2) panic("***  Panic  ***\n\
PS/2 controller uses the wrong scancode set: %u", scancode_set);
	kprintf("Scancode set: %u", scancode_set); */
	set_interrupt_callback(33, &handle_keyboard_input);
	enable_keyboard();
}

void enable_keyboard() {
	PS2_wait_writable();
	outb(PS2_COMMAND, 0xae);
	inb(KEYBOARD_COMMAND);
}

void disable_keyboard() {
	PS2_wait_writable();
	outb(PS2_COMMAND, 0xad);
}

uint8_t get_scan_code(bool *_make, bool *_special_key) {
	uint8_t input;
	do {
		input = keyboard_get_output();
		switch (input) {
			case 0xE0:
				*_special_key = true;
				break;
			default:
				if (input & 0x80) {
					*_make = false;
					return (input - 0x80);
				} else
					return input;
				break;
		}
	} while (input == 0xE0);
	return input; /* This should never be reached. */
}

char translate(int scancode) {
	if (State.caps || State.shift)
		return key_shift[scancode];
	else
		return key[scancode];
}


void handle_keyboard_input(struct Registers regs,
                           uint32_t error_code, 
                           uint32_t int_number) {
	bool make = true, special_key = false;
	int scancode;
	scancode = get_scan_code(&make, &special_key);
	switch (scancode) {
		case 0x1d:
			State.control = make;
			break;
		case 0x38:
			State.alt = make;
			break;
		case 0x2a:
		case 0x36:
			State.shift = make;
			break;
		case 0x3a:
			if (make)
				State.caps = !State.caps;
			break;
		case 0x45:
			if (make) State.numlock = !State.numlock;
			break;
		default:
			if ((make && !special_key) && (key[scancode] != '\0'))
				putchar(translate(scancode));
	}
}
