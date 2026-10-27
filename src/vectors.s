.section .text
.align 11 // 2^11; 2048-byte alignment
.global vector_table

//-----------------------
//context saving
//-----------------------

.macro SAVE_GPRS
    stp x0,  x1,  [sp, #-16]!
    stp x2,  x3,  [sp, #-16]!
    stp x4,  x5,  [sp, #-16]!
    stp x6,  x7,  [sp, #-16]!
    stp x8,  x9,  [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!

    stp x30, xzr, [sp, #-16]!
.endm

.macro RESTORE_GPRS
    ldp x30, xzr, [sp], #16

    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8,  x9,  [sp], #16
    ldp x6,  x7,  [sp], #16
    ldp x4,  x5,  [sp], #16
    ldp x2,  x3,  [sp], #16
    ldp x0,  x1,  [sp], #16
.endm

//-----------------------
//vector table
//-----------------------

vector_table:
    //current EL, using SP_EL0 as the stack pointer - exception originated at the same exception level at which it is handled
    b sync_sp0 //synchronous exception, arm64 fixed instruction length of 4 bytes
    .space 124

    b irq_sp0 //interrupt req
    .space 124

    b fiq_sp0 //fast interrupt req
    .space 124

    b serr_sp0 //sys error
    .space 124

    //current EL, using SP_ELx (current EL as the stack pointer) - exception originated at the same exception level at which it is handled
    b sync_spx
    .space 124

    b irq_spx
    .space 124

    b fiq_spx
    .space 124

    b serr_spx
    .space 124

    //lower EL, AArch64 - originating at a lower EL, with lower level code executing arm64 instructions
    b sync_lower
    .space 124

    b irq_lower
    .space 124

    b fiq_lower
    .space 124

    b serr_lower
    .space 124

    //lower EL, AArch32 - exceptions from a lower EL running arm32
    b sync_lower32
    .space 124

    b irq_lower32
    .space 124

    b fiq_lower32
    .space 124

    b serr_lower32
    .space 124

sync_spx:

    SAVE_GPRS

    mrs x0, esr_el1
    mrs x1, elr_el1
    mrs x2, far_el1

    bl exception_handler

    RESTORE_GPRS

    eret


sync_sp0:
irq_sp0:
fiq_sp0:
serr_sp0:
irq_spx:
fiq_spx:
serr_spx:
sync_lower:
irq_lower:
fiq_lower:
serr_lower:
sync_lower32:
irq_lower32:
fiq_lower32:
serr_lower32:
    b .
