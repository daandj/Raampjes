AS = nasm

test: main.asm
	$(AS) -f bin -o $@ $<

all: test

clean:
	-rm test

.PHONY: all clean
