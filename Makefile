all: kernel.elf

boot.o: src/boot.s
	aarch64-elf-as src/boot.s -o boot.o

vectors.o: src/vectors.s
	aarch64-elf-as src/vectors.s -o vectors.o

timer.o: src/timer.s
	aarch64-elf-as src/timer.s -o timer.o

kernel.o: src/kernel.c
	aarch64-elf-gcc -ffreestanding -c src/kernel.c -o kernel.o

kernel.elf: boot.o vectors.o timer.o kernel.o src/linker.ld
	aarch64-elf-ld -nostdlib -T src/linker.ld boot.o vectors.o timer.o kernel.o -o kernel.elf

run: kernel.elf
	qemu-system-aarch64 -machine virt -cpu cortex-a57 -kernel kernel.elf -nographic

clean:
	rm -f boot.o vectors.o timer.o kernel.o kernel.elf