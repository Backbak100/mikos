ENTRY(_start)
SECTIONS {
    . = 0x40100000; /* qemu loads in between 0x400000000 and 0x40100000- ram starts 0x400000000*/
    .startup . : { boot.o(.text) } /*the startup uses the executable of boot.s*/
    .text : { *(.text) } /*text from all files are put into .text*/
    .data : { *(.data) } /*data (variable assignments) are put into .data*/
    .bss : { *(.bss COMMON) }
    . = ALIGN(16); /* two times pointer size */
    . += 0x1000; /* reserves 4kB for the stack */
    stack_top = .;
    kernel_end = .;
}