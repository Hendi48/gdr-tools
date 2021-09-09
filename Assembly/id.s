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

.equ TICKLE_555, 0x90000555
.equ TICKLE_2AA, 0x900002AA

_start:
    add     -32, SP

    # This temporarily disables flash encryption, making reads fetch the real content from flash
    mov     1, D0
    call    IOConf0, [], 0
    mov     1, D0
    call    IOConf1, [], 8

    call    ReadSiliconID, [], 0

    # Reenable when we return to drive code
    clr     D0
    call    IOConf1, [], 8
    clr     D0
    call    IOConf0, [], 0

    add     32, SP
    rets

ReadSiliconID:
    mov     0xAA, D0
    movbu   D0, (TICKLE_555)
    mov     0x55, D0
    movbu   D0, (TICKLE_2AA)
    mov     0x90, D0
    movbu   D0, (TICKLE_555)

    nop
    nop

    movbu   (0x90000000), D0
    movbu   (0x90000001), D1

    movbu   D0, (ATA_bytecount_lo)
    movbu   D1, (ATA_bytecount_hi)
    call    FailIoctl, [], 0
    
    # Reset
    mov     0xF0, D0
    movbu   D0, (TICKLE_555)
    
    rets

IOConf0:
    extbu   D0
    cmp     0, D0
    bne     IOConf0_L0
    bclr    1, (0xD9A8)
    bra     IOConf0_L1

  IOConf0_L0:
    bset    1, (0xD9A8)
  IOConf0_L1:
    rets

IOConf1:
    and     1, D0
    movbu   D0, (4,SP)
    clr     D0
    movbu   D0, (5,SP)
    mov     0x9A, D0
    mov     SP, A0
    inc4    A0
    mov     2, D1
    call    IOConf2, [D2], 4
    ret     [], 8

IOConf2:
    mov     0xDF00, A1
    setlb
    movbu   (0xD904), D2
    btst    0x80, D2
    lne
    bra     IOConf2_L1
    
  IOConf2_L0:
    movbu   (A0), D2
    inc     A0
    movbu   D2, (A1)
    inc     A1
  IOConf2_L1:
    mov     D1, D2
    extbu   D1
    add     0xFF, D1
    extbu   D2
    cmp     0, D2
    bne     IOConf2_L0
    movbu   D0, (0xDF28)
    retf    [D2], 4

FailIoctl:
    clr     D0
    movbu   D0, (ATA_error)
    mov     0x51, D0   # 0x50 for graceful failure
    movbu   D0, (ATA_status)
    mov     0x3, D0
    movbu   D0, (ATA_nsect_ireason)
    mov     0x1, D0
    movbu   D0, (ATA_InterruptFlag)
    rets
