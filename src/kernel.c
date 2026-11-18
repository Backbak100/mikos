#include <stdint.h>
#include <stddef.h>
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

#define TOTAL_PAGES 8
#define BITMAP_SIZE ((TOTAL_PAGES + 7) / 8)

volatile uint8_t *uart = (uint8_t *) 0x09000000; //PL011 uart memory address mapped by qemu

extern char kernel_end[];

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

void print_address(void *address) {
    print_hex((uintptr_t)address);
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
-----------------------
PAGE ALLOCATOR FUNCTIONS
-----------------------
*/

uint8_t page_bitmap[BITMAP_SIZE];

int mark_page_taken(uint64_t i)  {
    if (i >= TOTAL_PAGES)
        return 0;
    
    page_bitmap[i] |= 1U;

    return 1;

}

int mark_page_free(uint64_t i) {
    if (i >= TOTAL_PAGES)
        return 0;
    
    //page_bitmap[i / 8] &= ~(1U << (i % 8));

    page_bitmap[i] ^= 1U;

    return 1;
}

int palloc_done(uint64_t i) {
    return (page_bitmap[i/8] >> (i % 8)) & 1;
}

void free_page(void *adr) {
    uintptr_t mem = (uintptr_t) adr;
    uintptr_t free_start = (((uintptr_t)(kernel_end)) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    uint64_t i = (mem - free_start) / PAGE_SIZE;

    mark_page_free(i); 

}

void *palloc(void) {

    for (uint64_t i = 0; i < TOTAL_PAGES; i++) {
        if (!palloc_done(i)) {

            mark_page_taken(i);
            uintptr_t free_start = (((uintptr_t)(kernel_end)) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
            
            return (void *)(free_start + i*PAGE_SIZE);
        }
    }

    return NULL;
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

unsigned long tick_seconds(void) {
    uint64_t x1 = get_timer_time();
    unsigned long x2;
    static unsigned long i = 0;

    while ((x2 = get_timer_time()) - x1 != get_timer_frequency());

    return ++i;
}

/*
----
MAIN
----
*/

void kmain(void) {
   print("hello world\n\n");

    __asm__ volatile("svc #0");

    
    print("\ncontinuation after the svc (exception)\n");


    
    print("\npalloc() address: ");
    void *adr = palloc();
    print_address(adr);
    print("bitmap: ");
    for (int i = 0; i<TOTAL_PAGES; i++){
        print_number(page_bitmap[i]);
        print(" ");
    }

    print("\n\nANOTHER PALLOC()\n");

    print("\npalloc() address: ");
    void *adr2 = palloc();
    print_address(adr2);
    print("bitmap: ");
    for (int i = 0; i<TOTAL_PAGES; i++){
        print_number(page_bitmap[i]);
        print(" ");
    }
    
    print("\n");

    print("\nFreeing page\n");
    free_page(adr);
    print("bitmap: ");
    for (int i = 0; i<TOTAL_PAGES; i++){
        print_number(page_bitmap[i]);
        print(" ");
    }
    print("\n\nFreeing page\n");
    free_page(adr2);
    print("bitmap: ");
    for (int i = 0; i<TOTAL_PAGES; i++){
        print_number(page_bitmap[i]);
        print(" ");
    }

    print("\n\n");

    print("Clock counter...\n");
    while (1) {
        int x = tick_seconds();
        print_number(x);
        print("\n");
        if (x >= 5)
            break;
    }

    print("wfi... ctrl A -> X to exit\n");
    
    while (1)
        __asm__ volatile("wfi");
}