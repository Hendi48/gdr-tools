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

    mov     1, D0
    call    IOConf0, [], 0
    mov     1, D0
    call    IOConf1, [], 0

    mov     0xAAAAAAAA, A0
    mov     0x10000, D0
    mov     FlashData, A1

    call    FlashIt, [D2], 8
    bra     Del

  Del0:
    nop
    inc     D0
  Del:
    mov     D0, (0x80001824)
    cmp     3200000, D0
    bcs     Del0

    clr     D0
    call    IOConf1, [], 0
    clr     D0
    call    IOConf0, [], 0

    clr     D0
    movbu   D0, (ATA_error)
    call    FailIoctl, [], 0

    bset    1, (0xD900)  # this resets the drive! gets stuck on next command without this

    add     32, SP
    rets

FlashIt:
    # A0: Dst Address
    # A1: Src Address
    # D0: Size
    mov     D0, D2

  FlashLoop:
    cmp     0, D2
    beq     FlashDone

    movbu   (A1), D0
    call    ProgramByte, [D2,D3], 12

    inc     A0
    inc     A1
    sub     1, D2
    bra     FlashLoop

  FlashDone:
    ret     [D2], 8

ProgramByte:
    # A0: Address
    # D0: Data
    mov     0, D3
    extbu   D0
  Retry:
    mov     0xAA, D1
    movbu   D1, (TICKLE_555)
    mov     0x55, D1
    movbu   D1, (TICKLE_2AA)
    mov     0xA0, D1
    movbu   D1, (TICKLE_555)

    movbu   D0, (A0)

    mov     D0, D2
    call    ResetMode, [], 0
    mov     D2, D0

    movbu   (A0), D1
    cmp     D2, D1
    beq     Done

    inc     D3
    cmp     10, D3
    ble     Retry

  Done:
    mov     D3, D1
    movbu   D1, (ATA_error)

    ret     [D2,D3], 12

ResetMode:
    movbu   (0x90000000), D0
    nop
    nop
    movbu   (0x90000000), D1
    nop
    nop
    cmp     D0, D1
    bne     ResetMode
    movbu   (0x90000000), D1
    nop
    nop
    cmp     D0, D1
    bne     ResetMode
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
    nop
    nop
    setlb
    movbu   (0xD904), D1
    btst    0x80, D1
    lne
    
    movbu   D0, (0xDF00)  # 0/1
    nop
    nop
    clr     D0
    movbu   D0, (0xDF01)
    nop
    nop
    mov     0x9A, D0
    movbu   D0, (0xDF28)

    rets

FailIoctl:
    mov     0x51, D0   # 0x50 for graceful failure
    movbu   D0, (ATA_status)
    mov     0x1, D0
    movbu   D0, (ATA_InterruptFlag)
    rets

FlashData:
    # to be appended by kernel
