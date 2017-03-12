KERNEL_C = $(wildcard kernel/*.c)
KERNEL_A = $(wildcard kernel/*.asm)
OBJS_C = $(subst kernel/,images/,$(subst .c,.o,$(KERNEL_C)))
OBJS_A = $(subst kernel/,images/,$(subst .asm,.o,$(KERNEL_A)))

CC = i686-raampjes-gcc
AS = nasm
CFLAGS = -std=gnu99 -ffreestanding -o2 -Wall -Wextra

.PHONY: all clean test libc libk install-headers

all: install-headers floppy.img

libc: 
	$(MAKE) -C libc/
	cp libc/build/i386/crt0.o sysroot/usr/lib/crt0.o
	cp libc/build/libc.a sysroot/usr/lib/libc.a
	cp libc/build/libk.a sysroot/usr/lib/libk.a

initrd/initrd.tar: libc
	$(MAKE) -C initrd/

install-headers:
	cp -R libc/include/* sysroot/usr/include/
	cp -R kernel/include/* sysroot/usr/include/

images/first_stage.img: boot/first_stage.asm
	$(AS) -f bin -o $@ $^

images/stage2.bin: boot/second_stage.asm
	$(AS) -f bin -o $@ $^

images/%.o: kernel/%.asm
	$(AS) -f elf -o $@ $^

images/%.o: kernel/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

images/initrd.tar: initrd/initrd.tar
	cp $^ $@

images/kernel.elf: $(OBJS_C) $(OBJS_A) libc
	$(CC) -T kernel/linker.ld -o $@ $(OBJS_C) $(OBJS_A) -lgcc -lk -nostdlib

images/floppy.img: images/first_stage.img 
	cp images/empty_floppy.img images/floppy.img
	dd conv=notrunc if=images/first_stage.img of=images/floppy.img

floppy.img: images/floppy.img images/stage2.bin images/kernel.elf images/initrd.tar
	hdiutil attach images/floppy.img
	cp images/stage2.bin /Volumes/BOOT\ FLOPPY/stage2.bin
	cp images/kernel.elf /Volumes/BOOT\ FLOPPY/kernel.elf
	cp images/initrd.tar /Volumes/BOOT\ FLOPPY/initrd.tar
	hdiutil detach /Volumes/BOOT\ FLOPPY/

test: floppy.img
	bochs -qf bochsrc

clean:
	-$(MAKE) -C initrd clean
	-rm images/first_stage.img
	-rm images/stage2.bin
	-rm images/floppy.img
	-rm bochsout.txt
	-rm $(OBJS_C) $(OBJS_A)
	-rm images/kernel.elf
	-rm images/initrd.tar
