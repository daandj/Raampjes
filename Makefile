first_stage.img: boot/first_stage.asm
	nasm -f bin -o images/first_stage.img boot/first_stage.asm

stage2.bin: boot/second_stage.asm
	nasm -f bin -o images/stage2.bin boot/second_stage.asm

fat_img: first_stage.img 
	cp images/empty_floppy.img images/floppy.img
	dd conv=notrunc if=images/first_stage.img of=images/floppy.img

floppy.img:	fat_img stage2.bin
	hdiutil attach images/floppy.img
	cp images/stage2.bin /Volumes/BOOT\ FLOPPY/stage2.bin
	hdiutil detach /Volumes/BOOT\ FLOPPY/

test: floppy.img
	bochs -qf bochsrc

clean:
	-rm images/first_stage.img
	-rm images/stage2.bin
	-rm images/floppy.img
	-rm bochsout.txt
