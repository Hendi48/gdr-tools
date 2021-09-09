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
    mov     0x10000, D0  # sector size (purely for verification check - sector layout depends on chip)
    call    EraseSector, [D2], 8

    call    FailIoctl, [], 0
    
    clr     D0
    call    IOConf1, [], 0
    clr     D0
    call    IOConf0, [], 0
    
    add     32, SP
    rets

EraseSector:
    # A0: Address
    mov     0, A1
  Retry:
    mov     0xAA, D1
    movbu   D1, (TICKLE_555)
    mov     0x55, D1
    movbu   D1, (TICKLE_2AA)
    mov     0x80, D1
    movbu   D1, (TICKLE_555)
    mov     0xAA, D1
    movbu   D1, (TICKLE_555)
    mov     0x55, D1
    movbu   D1, (TICKLE_2AA)
    mov     0x30, D1
    movbu   D1, (A0)

    mov     D0, D2

    call    ResetMode, [], 0

    mov     A0, D0
    mov     D2, D1
    call    VerifyErased, [D2,D3], 8

    movbu   D0, (ATA_error)

    ret     [D2], 8

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

VerifyErased:
    mov     D0, A1
    mov     1, D0
    clr     D3
    bra     VE_L2
    
  VE_L0:
    mov     A1, A0
    movbu   (A0), D2
    inc     A1
    cmp     0xFF, D2
    beq     VE_L1
    movbu   D3, (ATA_nsect_ireason)
    clr     D0
    bra     VE_L3
    
  VE_L1:
    inc     D3
  VE_L2:
    cmp     D1, D3
    bcs     VE_L0
  VE_L3:
    extbu   D0
    retf    [D2,D3], 8

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
