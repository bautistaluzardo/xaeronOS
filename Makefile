CC      = x86_64-elf-gcc
AS      = nasm
LD      = x86_64-elf-ld
CFLAGS  = -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
          -Wall -Wextra -O2
LDFLAGS = -nostdlib

# ========================= #
# objetos                   #
# ========================= #

OBJ = \
	obj/boot.o \
	obj/cpu/idt_asm.o \
	obj/cpu/idt.o \
	obj/drivers/vga.o \
	obj/drivers/timer.o \
	obj/drivers/keyboard.o \
	obj/lib/string.o \
	obj/shell/commands.o \
	obj/shell/shell.o \
	obj/mm/pmm.o \
	obj/mm/vmm.o \
	obj/kernel/kernel.o

# ========================= #
# targets principales       #
# ========================= #

all: myos.iso

# ========================= #
# asm                       #
# ========================= #

obj/boot.o: src/boot.asm
	mkdir -p obj
	$(AS) -f elf64 src/boot.asm -o obj/boot.o

obj/cpu/idt_asm.o: src/cpu/idt.asm
	mkdir -p obj/cpu
	$(AS) -f elf64 src/cpu/idt.asm -o obj/cpu/idt_asm.o

# ========================= #
# vmm                       #
# ========================= #

obj/mm/vmm.o: src/mm/vmm.c src/mm/vmm.h
	mkdir -p obj/mm
	$(CC) $(CFLAGS) -c src/mm/vmm.c -o obj/mm/vmm.o

# ========================= #
# pmm                       #
# ========================= #

obj/mm/pmm.o: src/mm/pmm.c src/mm/pmm.h
	mkdir -p obj/mm
	$(CC) $(CFLAGS) -c src/mm/pmm.c -o obj/mm/pmm.o

# ========================= #
# cpu                       #
# ========================= #

obj/cpu/idt.o: src/cpu/idt.c src/cpu/idt.h
	mkdir -p obj/cpu
	$(CC) $(CFLAGS) -c src/cpu/idt.c -o obj/cpu/idt.o

# ========================= #
# drivers                   #
# ========================= #

obj/drivers/vga.o: src/drivers/vga.c src/drivers/vga.h
	mkdir -p obj/drivers
	$(CC) $(CFLAGS) -c src/drivers/vga.c -o obj/drivers/vga.o

obj/drivers/timer.o: src/drivers/timer.c src/drivers/timer.h
	mkdir -p obj/drivers
	$(CC) $(CFLAGS) -c src/drivers/timer.c -o obj/drivers/timer.o

obj/drivers/keyboard.o: src/drivers/keyboard.c src/drivers/keyboard.h
	mkdir -p obj/drivers
	$(CC) $(CFLAGS) -c src/drivers/keyboard.c -o obj/drivers/keyboard.o

# ========================= #
# lib                       #
# ========================= #

obj/lib/string.o: src/lib/string.c src/lib/string.h
	mkdir -p obj/lib
	$(CC) $(CFLAGS) -c src/lib/string.c -o obj/lib/string.o

# ========================= #
# shell                     #
# ========================= #

obj/shell/commands.o: src/shell/commands.c src/shell/commands.h
	mkdir -p obj/shell
	$(CC) $(CFLAGS) -c src/shell/commands.c -o obj/shell/commands.o

obj/shell/shell.o: src/shell/shell.c src/shell/shell.h
	mkdir -p obj/shell
	$(CC) $(CFLAGS) -c src/shell/shell.c -o obj/shell/shell.o

# ========================= #
# kernel                    #
# ========================= #

obj/kernel/kernel.o: src/kernel/kernel.c
	mkdir -p obj/kernel
	$(CC) $(CFLAGS) -c src/kernel/kernel.c -o obj/kernel/kernel.o

# ========================= #
# link                      #
# ========================= #

kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) -T linker.ld -o kernel.bin $(OBJ)

# ========================= #
# iso                       #
# ========================= #

myos.iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	cp boot/grub/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso iso

# ========================= #
# run / clean               #
# ========================= #

run: myos.iso
	qemu-system-x86_64 -cdrom myos.iso

clean:
	rm -rf obj iso kernel.bin myos.iso