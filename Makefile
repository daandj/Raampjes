bootloader.img: bootloader.asm
	nasm -f bin -o first_stage.img first_stage.asm

exports:
	export PATH="$$PATH:$$HOME/opt/bochs/bin"
	export BXSHARE="$$HOME/opt/bochs/share/bochs"

floppy.img: first_stage.img
	dd bs=512 count=2880 if=/dev/zero of=floppy.img
	dd conv=notrunc if=first_stage.img of=floppy.img

test: floppy.img
	bochs -qf bochsrc

clean:
	-rm first_stage.img
	-rm floppy.img



