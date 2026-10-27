#include <stdint.h>

volatile uint8_t *uart = (uint8_t *) 0x09000000; //PL011 uart memory address mapped by qemu

void putchar(char c) {
    *uart = c;
}

void print(const char *s) {
    while(*s != '\0') {
        putchar(*s);
        s++;
    }
}

void print_hex(uint64_t value) {
    const char *hex = "0123456789abcdef";

    print("0x");

    for (int i = 60; i >= 0; i -= 4) {
        putchar(hex[(value >> i) & 0xf]);
    }

    print("\n");
}

//x0, x1, x2
void exception_handler(uint64_t esr, uint64_t elr, uint64_t far) {
    print("EXCEPTION\n");

    print("ESR_EL1: ");
    print_hex(esr);

    print("ELR_EL1: ");
    print_hex(elr);

    print("FAR_EL1: ");
    print_hex(far);

}

void kmain(void) {
    print("hello world\n");

    __asm__ volatile("svc #0");

    print("\nafter the svc\nctrl A -> X to exit\n");

    while (1)
        __asm__ volatile("wfi");
}