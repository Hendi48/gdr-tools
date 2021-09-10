    .mn10300
    .text
    .section .cache,"ax",@progbits
    .globl _start
    #.align 4

.equ ATA_InterruptFlag, 0xDA0C
.equ ATA_error, 0xDA24
.equ ATA_nsect_ireason, 0xDA28
.equ ATA_secnum, 0xDA2C
.equ ATA_bytecount_lo, 0xDA30
.equ ATA_bytecount_hi, 0xDA34
.equ ATA_status, 0xDA3C

_start:
    add     -32, SP

    movbu   (0x90000000), D0
    movbu   D0, (ATA_nsect_ireason)
    movbu   (0x90000001), D0
    movbu   D0, (ATA_secnum)
    movbu   (0x90000002), D0
    movbu   D0, (ATA_bytecount_lo)
    movbu   (0x90000003), D0
    movbu   D0, (ATA_bytecount_hi)
    call    FailIoctl, [], 0
    
    add     32, SP
    rets

FailIoctl:
    clr     D0
    movbu   D0, (ATA_error)
    mov     0x51, D0
    movbu   D0, (ATA_status)
    mov     0x1, D0
    movbu   D0, (ATA_InterruptFlag)
    rets
