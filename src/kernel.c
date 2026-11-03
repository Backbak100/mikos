#include <stdint.h>

volatile uint8_t *uart = (uint8_t *) 0x09000000; //PL011 uart memory address mapped by qemu

extern unsigned long get_timer_frequency(void);
extern unsigned long get_timer_time(void);

/*
-----------------------
VARIOUS PRINT FUNCTIONS
-----------------------
*/

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

void print_number(uint64_t value) {
    int i = 0;
    char str[21];

    if (value == 0) {
        putchar('0');
    }

    while (value > 0) {
        str[i++] = (value % 10) + '0';
        value /= 10;
    }

    while (i > 0) {
        putchar(str[--i]);
    }
}

/*
----------
EXCEPTIONS
----------
*/

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

void default_handler(uint64_t esr, uint64_t elr, uint64_t far) {
    print("EXCEPTION\n");

    print("ESR_EL1: ");
    print_hex(esr);

    print("ELR_EL1: ");
    print_hex(elr);

    print("FAR_EL1: ");
    print_hex(far);
}

/*
---------------
TIMER FUNCTIONS
---------------
*/

/*
extern unsigned long get_timer_frequency(void);
extern unsigned long get_timer_time(void);
*/

void timer_init(void) {
    unsigned long frequency = get_timer_frequency();
    
    print_number(frequency);
}

//frq = 62500000

int tick_seconds(void) {
    uint64_t x1 = get_timer_time();
    int x2;
    static int i = 0;

    while ((x2 = get_timer_time()) - x1 != get_timer_frequency());

    return ++i;
}

/*
----
MAIN
----
*/

void kmain(void) {
    print("hello world\n");

    __asm__ volatile("svc #0");

    print("\ncontinuation after the svc (exception)\nctrl A -> X to exit\n");

    timer_init();

    print("\ntimer init completed\ntimer time: ");

    print_number(get_timer_time());

    print("\nget timer time completed\n");

    while (1) {
        print_number(tick_seconds());
        print("\n");
    }
    
    while (1)
        __asm__ volatile("wfi");
}