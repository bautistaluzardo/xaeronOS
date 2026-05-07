CC      = x86_64-elf-gcc
AS      = nasm
LD      = x86_64-elf-ld

CFLAGS  = -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
          -Wall -Wextra -O2
LDFLAGS = -nostdlib

all: myos.iso

obj/boot.o: src/boot.asm
	mkdir -p obj
	$(AS) -f elf64 src/boot.asm -o obj/boot.o

obj/kernel.o: src/kernel.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c src/kernel.c -o obj/kernel.o

kernel.bin: obj/boot.o obj/kernel.o
	$(LD) $(LDFLAGS) -T linker.ld -o kernel.bin obj/boot.o obj/kernel.o

myos.iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	cp boot/grub/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso iso

run: myos.iso
	qemu-system-x86_64 -cdrom myos.iso

clean:
	rm -rf obj iso kernel.bin myos.iso
