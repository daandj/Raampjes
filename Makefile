KERNEL_C = $(wildcard kernel/*.c)
KERNEL_A = $(wildcard kernel/*.asm)
OBJS_C = $(subst kernel/,images/,$(subst .c,.o,$(KERNEL_C)))
OBJS_A = $(subst kernel/,images/,$(subst .asm,.o,$(KERNEL_A)))
CC = i686-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -o2 -Wall -Wextra -nostdlib -Ikernel/include/

images/first_stage.img: boot/first_stage.asm
	nasm -f bin -o images/first_stage.img boot/first_stage.asm

images/stage2.bin: boot/second_stage.asm
	nasm -f bin -o images/stage2.bin boot/second_stage.asm

images/%.o: kernel/%.asm
	nasm -f elf -o $@ $^

images/%.o: kernel/%.c
	echo $^
	$(CC) $(CFLAGS) -c $^ -o $@

images/kernel.elf: $(OBJS_C) $(OBJS_A)
	echo $^
	i686-elf-gcc -T kernel/linker.ld -ffreestanding -o2 -o $@ -nostdlib $^ -lgcc

images/floppy.img: images/first_stage.img 
	cp images/empty_floppy.img images/floppy.img
	dd conv=notrunc if=images/first_stage.img of=images/floppy.img

floppy.img: images/floppy.img images/stage2.bin images/kernel.elf
	hdiutil attach images/floppy.img
	cp images/stage2.bin /Volumes/BOOT\ FLOPPY/stage2.bin
	cp images/kernel.elf /Volumes/BOOT\ FLOPPY/kernel.elf
	hdiutil detach /Volumes/BOOT\ FLOPPY/

test: floppy.img
	bochs -qf bochsrc

clean:
	-rm images/first_stage.img
	-rm images/stage2.bin
	-rm images/floppy.img
	-rm bochsout.txt
	-rm $(OBJS_C) $(OBJS_A)
	-rm images/kernel.elf
