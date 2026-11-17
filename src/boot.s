.global _start
_start:
    ldr x0, =stack_top
    mov sp, x0

    //install the exception vector table
    ldr x0, =vector_table
    msr VBAR_EL1, x0
    isb

    bl kmain
