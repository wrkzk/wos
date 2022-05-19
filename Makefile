C_SOURCES = $(wildcard kernel/*.c libc/*.c drivers/*.c cpu/*.c)
HEADERS = $(wildcard kernel/*.h libc/*.h drivers/*.h cpu/*.h)
OBJ = $(C_SOURCES:.c=.o)

CC = i686-elf-gcc
CFLAGS = -ffreestanding -O2 -nostdlib -Wall -std=gnu99 -Wextra

AS = nasm
ASFLAGS = -felf32

kernel.bin: boot/boot.o kernel/kernel_entry.o $(OBJ)
	$(CC) -T linker.ld -o $@ $(CFLAGS) $^ -lgcc

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c kernel/kernel_entry.o ${HEADERS}
	$(CC) $(CFLAGS) -c $< -o $@

run: kernel.bin
	qemu-system-i386 -kernel kernel.bin

clean:
	rm -rf *.bin *.o
	rm -rf kernel/*.o drivers/*.o cpu/*.o libc/*.o