##########################################################
# Reversed Hitachi flasher microcode for mn103 IDE drives.
# Assembled result is byte-identical with original binary.
##########################################################

    .mn10300
    .text
    .section .cache,"ax",@progbits
    .globl _start

.equ byte_D900, 0xD900
.equ byte_D904, 0xD904
.equ byte_D905, 0xD905

.equ byte_D950, 0xD950
.equ byte_D954, 0xD954
.equ byte_D95C, 0xD95C

.equ byte_D9A0, 0xD9A0
.equ byte_D9A8, 0xD9A8

.equ byte_D9A8, 0xD9A8

# ATA
.equ ATA_DA04, 0xDA04
.equ ATA_InterruptFlag, 0xDA0C
.equ ATA_DA18, 0xDA18
.equ ATA_taskfile_error, 0xDA24
.equ ATA_taskfile_seccount, 0xDA28
.equ ATA_taskfile_secnum, 0xDA2C
.equ ATA_taskfile_bytecount_lo, 0xDA30
.equ ATA_taskfile_bytecount_hi, 0xDA34
.equ ATA_taskfile_status, 0xDA3C
.equ ATA_DA40, 0xDA40
.equ ATA_DA44, 0xDA44
.equ ATA_DA48, 0xDA48
.equ ATA_DA4C, 0xDA4C
.equ ATA_DA58, 0xDA58
.equ ATA_DA5C, 0xDA5C
.equ ATA_DAAC, 0xDAAC

.equ byte_DF28, 0xDF28
.equ byte_DF29, 0xDF29
.equ byte_DF30, 0xDF30
.equ byte_DF31, 0xDF31
.equ byte_DF32, 0xDF32
.equ byte_DF33, 0xDF33
.equ byte_DF34, 0xDF34
.equ byte_DF35, 0xDF35
.equ byte_DF36, 0xDF36
.equ byte_DF37, 0xDF37
.equ byte_DF38, 0xDF38
.equ byte_DF39, 0xDF39
.equ byte_DF3A, 0xDF3A
.equ byte_DF3B, 0xDF3B
.equ byte_DF3C, 0xDF3C
.equ byte_DF3D, 0xDF3D
.equ byte_DF3E, 0xDF3E
.equ byte_DF3F, 0xDF3F

.equ Ctrl_CDB_0, 0xDFA0
.equ Ctrl_CDB_1, 0xDFA1
.equ Ctrl_CDB_2, 0xDFA2
.equ Ctrl_CDB_3, 0xDFA3
.equ Ctrl_CDB_4, 0xDFA4
.equ Ctrl_CDB_5, 0xDFA5
.equ Ctrl_CDB_6, 0xDFA6
.equ Ctrl_CDB_7, 0xDFA7
.equ Ctrl_CDB_8, 0xDFA8
.equ Ctrl_CDB_9, 0xDFA9
.equ Ctrl_CDB_A, 0xDFAA
.equ Ctrl_CDB_B, 0xDFAB

.equ byte_DFB0, 0xDFB0

# Cache
.equ buf_80002000_flash, 0x80002000

.equ unk_8000B000, 0x8000B000
.equ unk_8000B080, 0x8000B080

.equ unk_80010000, 0x80010000
.equ CDB_in_0, 0x80010010
.equ CDB_in_1, 0x80010011
.equ CDB_in_2, 0x80010012
.equ CDB_in_3, 0x80010013
.equ CDB_in_4, 0x80010014
.equ CDB_in_5, 0x80010015
.equ CDB_in_6, 0x80010016
.equ CDB_in_7, 0x80010017
.equ CDB_in_8, 0x80010018
.equ CDB_in_9, 0x80010019
.equ CDB_in_A, 0x8001001A
.equ CDB_in_B, 0x8001001B
.equ byte_in_20, 0x80010020
.equ byte_in_21, 0x80010021
.equ unk_80010030, 0x80010030
.equ byte_80010040, 0x80010040


# Magic flash addresses
.equ FLASH_BASE, 0x90000000
.equ TICKLE_2AA, 0x900002AA
.equ TICKLE_555, 0x90000555
.equ TICKLE_2AAA, 0x90002AAA
.equ TICKLE_5555, 0x90005555


# Hack for GNU as encoding length problem
.macro bclr_s bit, addr
    .byte 0xFE, 0x81, (\addr) & 0xFF, ( (\addr) >> 8 ) & 0xFF, \bit
.endm
.macro bset_s bit, addr
    .byte 0xFE, 0x80, (\addr) & 0xFF, ( (\addr) >> 8 ) & 0xFF, \bit
.endm


# .org 0x80000000

# =============== S U B R O U T I N E =======================================


_start:
                movbu   (CDB_in_0), D0
                cmp     0x55, D0
                bne     Not55_1
                movbu   (CDB_in_6), D0
                cmp     1, D0
                bne     Not55_1
                movbu   (CDB_in_B), D0
                mov     1, D1
                movbu   D1, (g_EntrypointCmdMode)
                cmp     D1, D0
                beq     loc_8000002C
                clr     D0
                movbu   D0, (g_EntrypointCmdMode)

loc_8000002C:                           # CODE XREF: Entrypoint+23↑j
                call    Cmd55, [D2,A2], 0x14
                bra     locret_80000050
# ---------------------------------------------------------------------------

Not55_1:                                # CODE XREF: Entrypoint+8↑j
                                        # Entrypoint+12↑j
                movbu   (CDB_in_6), D0
                mov     1, D1
                movbu   D1, (g_EntrypointCmdMode)
                cmp     D1, D0
                beq     loc_8000004B
                clr     D0
                movbu   D0, (g_EntrypointCmdMode)

loc_8000004B:                           # CODE XREF: Entrypoint+42↑j
                call    CmdNot55, [D2,D3,A2,A3], 0x1C

locret_80000050:                        # CODE XREF: Entrypoint+31↑j
                ret     [], 4
# End of function Entrypoint


# =============== S U B R O U T I N E =======================================


Cmd55:                                  # CODE XREF: Entrypoint:loc_8000002C↑p
                mov     1, D0
                call    IOConf0, [], 0
                mov     1, D0
                call    IOConf1, [], 8
                mov     g_FlashVendorID, D0
                mov     D0, (4,SP)
                mov     g_FlashDeviceID, D0
                mov     D0, (8,SP)
                mov     g_FlashTypeCol3, A0
                mov     g_FlashTypeID_Always2, A1
                mov     g_FlashTypeCol4, D0
                mov     g_FlashAddressingType, D1
                call    CheckFlashType, [D2,D3,A2,A3], 0x2C
                extbu   D0
                cmp     0xFF, D0
                bne     FlashTypeOK
                mov     0xC4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

UnsupportedFlash:                       # CODE XREF: Cmd55:UnsupportedFlash↓j
                bra     UnsupportedFlash
# ---------------------------------------------------------------------------

FlashTypeOK:                            # CODE XREF: Cmd55+40↑j
                movbu   (g_FlashTypeID_Always2), D0
                call    SetD9A0, [], 0
                movbu   (g_FlashVendorID), D0
                mov     ROMMap_Atmel_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0x1F, D0
                beq     loc_80000101
                mov     ROMMap_Macronix_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0xC2, D0
                beq     loc_80000101
                cmp     0x37, D0
                beq     loc_80000101
                mov     ROMMap_SST_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0xBF, D0
                beq     loc_80000101
                mov     ROMMap_PMC_4000, A0
                mov     A0, (g_pROMMap)

loc_80000101:                           # CODE XREF: Cmd55+78↑j Cmd55+8A↑j ...
                movbu   (8,A0), D2
                mov     (A0), D1
                asl     0xA, D2
                mov     D2, A0          # 2000
                mov     unk_8000B000, D0
                call    SetATABytecount, [A2], 4
                mov     unk_8000B080, A2
                mov     1, D0
                movbu   D0, (A2)
                movbu   (0x80010022), D0
                movbu   D0, (0x8000B081)
                movbu   (byte_in_20), D0
                movbu   D0, (0x8000B082)
                movbu   (byte_in_21), D0
                movbu   D0, (0x8000B083)
                mov     (0x8000B080), D1
                mov     0x8B61D2AE, D0  # What's this???
                call    TransferSomeBytes, [D2], 0x18
                mov     D0, (A2)
                call    Do_ATA_SuccessRes, [], 4
                clr     D0
                movbu   D0, (g_bWantErase)
                movbu   D0, (g_bReallyWantErase)
                mov     unk_80010000, A0
                bra     loc_80000174
# ---------------------------------------------------------------------------

loc_8000016F:                           # CODE XREF: Cmd55+124↓j
                clr     D1
                movbu   D1, (A0)
                inc     A0
                inc     D0

loc_80000174:                           # CODE XREF: Cmd55+11A↑j
                extbu   D0
                cmp     0x10, D0
                blt     loc_8000016F
                mov     unk_80010030, A0
                clr     D0
                bra     loc_80000187
# ---------------------------------------------------------------------------

loc_80000182:                           # CODE XREF: Cmd55+137↓j
                clr     D1
                movbu   D1, (A0)
                inc     A0
                inc     D0

loc_80000187:                           # CODE XREF: Cmd55+12D↑j
                extbu   D0
                cmp     0x20, D0
                blt     loc_80000182
                clr     D0
                movbu   D0, (g_bAllowErase)

loc_80000193:                           # CODE XREF: Cmd55+170↓j
                movbu   (byte_DFB0), D0
                movbu   (byte_D950), D1
                btst    8, D0
                beq     loc_800001A5
                call    ClearATAStatus, [], 0
                bra     loc_800001BB
# ---------------------------------------------------------------------------

loc_800001A5:                           # CODE XREF: Cmd55+149↑j
                btst    0x80, D1
                beq     loc_800001B1
                call    ATAFlagCheck, [], 4
                bra     loc_800001BB
# ---------------------------------------------------------------------------

loc_800001B1:                           # CODE XREF: Cmd55+155↑j
                btst    0x40, D1
                beq     loc_800001BB
                call    SCSI_Dispatch, [], 4

loc_800001BB:                           # CODE XREF: Cmd55+150↑j Cmd55+15C↑j ...
                movbu   (g_bAllowErase), D0
                cmp     1, D0
                bne     loc_80000193
                mov     1, D0
                movbu   D0, (g_bWantErase)
                movbu   D0, (g_bReallyWantErase)

loc_800001D3:                           # CODE XREF: Cmd55+190↓j Cmd55+19C↓j ...
                movbu   (byte_DFB0), D0
                movbu   (byte_D950), D1
                btst    8, D0
                beq     loc_800001E5
                call    ClearATAStatus, [], 0
                bra     loc_800001D3
# ---------------------------------------------------------------------------

loc_800001E5:                           # CODE XREF: Cmd55+189↑j
                btst    0x80, D1
                beq     loc_800001F1
                call    ATAFlagCheck, [], 4
                bra     loc_800001D3
# ---------------------------------------------------------------------------

loc_800001F1:                           # CODE XREF: Cmd55+195↑j
                btst    0x40, D1
                beq     loc_800001D3
                call    SCSI_Dispatch, [], 4
                bra     loc_800001D3
# End of function Cmd55


# =============== S U B R O U T I N E =======================================


ClearATAStatus:                         # CODE XREF: Cmd55+14B↑p Cmd55+18B↑p
                bclr_s  8, byte_DFB0
                clr     D0
                movbu   D0, (ATA_taskfile_status)
                rets
# End of function ClearATAStatus


# =============== S U B R O U T I N E =======================================


ATAFlagCheck:                           # CODE XREF: Cmd55+157↑p Cmd55+197↑p
                mov     0x80, D0
                movbu   D0, (byte_D950)
                movbu   (byte_DFB0), D0
                btst    0x80, D0
                beq     locret_8000021B
                call    PossiblyRaiseATAError, [], 4

locret_8000021B:                        # CODE XREF: ATAFlagCheck+C↑j
                ret     [], 4
# End of function ATAFlagCheck


# =============== S U B R O U T I N E =======================================


PossiblyRaiseATAError:                  # CODE XREF: ATAFlagCheck+E↑p
                bclr_s  0x80, byte_DFB0
                movbu   (ATA_taskfile_status), D0
                cmp     8, D0
                bne     loc_80000236
                mov     0xFF, D0
                movbu   D0, (ATA_DA18)
                clr     D0
                movbu   D0, (ATA_taskfile_status)
                bra     locret_8000023D
# ---------------------------------------------------------------------------

loc_80000236:                           # CODE XREF: PossiblyRaiseATAError+A↑j
                mov     4, D0  # IDNF
                call    ATA_ErrorRes, [], 0

locret_8000023D:                        # CODE XREF: PossiblyRaiseATAError+16↑j
                ret     [], 4
# End of function PossiblyRaiseATAError


# =============== S U B R O U T I N E =======================================


SCSI_Dispatch:                          # CODE XREF: Cmd55+163↑p Cmd55+1A3↑p
                mov     0x40, D0
                movbu   D0, (byte_D950)
                mov     1, D0
                movbu   D0, (ATA_DA5C)
                movbu   (Ctrl_CDB_0), D0
                movbu   D0, (CDB_in_0)
                movbu   (Ctrl_CDB_1), D0
                movbu   D0, (CDB_in_1)
                movbu   (Ctrl_CDB_2), D0
                movbu   D0, (CDB_in_2)
                movbu   (Ctrl_CDB_3), D0
                movbu   D0, (CDB_in_3)
                movbu   (Ctrl_CDB_4), D0
                movbu   D0, (CDB_in_4)
                movbu   (Ctrl_CDB_5), D0
                movbu   D0, (CDB_in_5)
                movbu   (Ctrl_CDB_6), D0
                movbu   D0, (CDB_in_6)
                movbu   (Ctrl_CDB_7), D0
                movbu   D0, (CDB_in_7)
                movbu   (Ctrl_CDB_8), D0
                movbu   D0, (CDB_in_8)
                movbu   (Ctrl_CDB_9), D0
                movbu   D0, (CDB_in_9)
                movbu   (Ctrl_CDB_A), D0
                movbu   D0, (CDB_in_A)
                movbu   (Ctrl_CDB_B), D0
                movbu   D0, (CDB_in_B)
                movbu   (CDB_in_0), D0
                cmp     0x3B, D0
                bne     loc_800002C7
                call    SCSI_3B_WriteBuffer, [D2,D3,A2], 0x10
                bra     locret_80000310
# ---------------------------------------------------------------------------

loc_800002C7:                           # CODE XREF: SCSI_Dispatch+7E↑j
                movbu   (CDB_in_0), D0
                cmp     0x5A, D0
                bne     loc_800002D8
                call    SCSI_5A_ModeSense, [], 4
                bra     locret_80000310
# ---------------------------------------------------------------------------

loc_800002D8:                           # CODE XREF: SCSI_Dispatch+8F↑j
                movbu   (CDB_in_0), D0
                cmp     0, D0
                bne     loc_800002E9
                call    SCSI_0_TestUnitReady, [], 4
                bra     locret_80000310
# ---------------------------------------------------------------------------

loc_800002E9:                           # CODE XREF: SCSI_Dispatch+A0↑j
                movbu   (CDB_in_0), D0
                cmp     3, D0
                bne     loc_800002FA
                call    SCSI_3_RequestSense, [], 4
                bra     locret_80000310
# ---------------------------------------------------------------------------

loc_800002FA:                           # CODE XREF: SCSI_Dispatch+B1↑j
                movbu   (CDB_in_0), D0
                cmp     0x4A, D0
                bne     loc_8000030B
                call    SCSI_4A_GetEventStatusNotification, [D2], 8
                bra     locret_80000310
# ---------------------------------------------------------------------------

loc_8000030B:                           # CODE XREF: SCSI_Dispatch+C2↑j
                call    SCSI_Unhandled, [], 4

locret_80000310:                        # CODE XREF: SCSI_Dispatch+85↑j
                                        # SCSI_Dispatch+96↑j ...
                ret     [], 4
# End of function SCSI_Dispatch


# =============== S U B R O U T I N E =======================================


SCSI_Unhandled:                         # CODE XREF: SCSI_Dispatch:loc_8000030B↑p
                mov     2, D0
                movbu   D0, (0x80010002)
                mov     0x3A, D0
                movbu   D0, (0x8001000C)
                clr     D0
                movbu   D0, (0x8001000D)
                mov     0x24, D0  # ABRT|IDNF
                call    ATA_ErrorRes, [], 0
                ret     [], 4
# End of function SCSI_Unhandled


# =============== S U B R O U T I N E =======================================


SCSI_0_TestUnitReady:                   # CODE XREF: SCSI_Dispatch+A2↑p
                mov     2, D0
                movbu   D0, (0x80010002)
                mov     0x3A, D0
                movbu   D0, (0x8001000C)
                clr     D0
                movbu   D0, (0x8001000D)
                mov     0x24, D0  # ABRT|IDNF
                call    ATA_ErrorRes, [], 0
                ret     [], 4
# End of function SCSI_0_TestUnitReady


# =============== S U B R O U T I N E =======================================


SCSI_3_RequestSense:                    # CODE XREF: SCSI_Dispatch+B3↑p
                movbu   (CDB_in_4), D0
                cmp     0, D0
                bne     loc_80000366
                call    ATA_SuccessRes, [], 0
                bra     locret_800003AB
# ---------------------------------------------------------------------------

loc_80000366:                           # CODE XREF: SCSI_3_RequestSense+8↑j
                movbu   (CDB_in_4), D0
                cmp     0x12, D0
                blt     loc_80000378
                mov     0x12, D0
                movbu   D0, (CDB_in_4)

loc_80000378:                           # CODE XREF: SCSI_3_RequestSense+19↑j
                movbu   (CDB_in_4), D0
                mov     unk_80010000, D1
                movbu   D0, (0x80010007)
                movbu   (CDB_in_4), D0
                mov     D0, A0
                clr     D0
                call    ATA_Transfer, [D2,D3], 0x10
                extbu   D0
                cmp     1, D0
                bne     loc_800003A4
                call    ATA_SuccessRes, [], 0
                bra     locret_800003AB
# ---------------------------------------------------------------------------

loc_800003A4:                           # CODE XREF: SCSI_3_RequestSense+46↑j
                mov     4, D0  # IDNF
                call    ATA_ErrorRes, [], 0

locret_800003AB:                        # CODE XREF: SCSI_3_RequestSense+F↑j
                                        # SCSI_3_RequestSense+4D↑j
                ret     [], 4
# End of function SCSI_3_RequestSense


# =============== S U B R O U T I N E =======================================


SCSI_4A_GetEventStatusNotification:     # CODE XREF: SCSI_Dispatch+C4↑p
                movbu   (CDB_in_7), D1
                movbu   (CDB_in_8), D0
                and     0xFF, D1
                mov     D1, D2
                asl     8, D2
                and     0xFF, D0
                or      D0, D2
                mov     D2, D1
                exthu   D1
                cmp     0, D1
                bne     loc_800003D4
                call    ATA_SuccessRes, [], 0
                ret     [D2], 8
# ---------------------------------------------------------------------------

loc_800003D4:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+1C↑j
                cmp     8, D1
                blt     loc_800003DA
                mov     8, D2

loc_800003DA:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+28↑j
                clr     D0
                movbu   D0, (0x80010040)
                mov     6, D0
                movbu   D0, (0x80010041)
                mov     0x5E, D0
                movbu   D0, (0x80010043)
                clr     D0
                movbu   D0, (0x80010046)
                movbu   D0, (0x80010047)
                movbu   (CDB_in_4), D1
                mov     2, D0
                and     0x5E, D1
                extbu   D1
                and     D1, D0
                cmp     2, D0
                bne     loc_80000414
                mov     1, D0
                bra     loc_80000450
# ---------------------------------------------------------------------------

loc_80000414:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+60↑j
                mov     4, D0
                and     D1, D0
                cmp     4, D0
                bne     loc_8000042F
                mov     2, D0
                movbu   D0, (0x80010042)
                clr     D0
                movbu   D0, (0x80010044)
                mov     3, D0
                bra     loc_8000045D
# ---------------------------------------------------------------------------

loc_8000042F:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+6C↑j
                mov     8, D0
                and     D1, D0
                cmp     8, D0
                bne     loc_8000043B
                mov     3, D0
                bra     loc_80000450
# ---------------------------------------------------------------------------

loc_8000043B:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+87↑j
                mov     0x10, D0
                and     D1, D0
                cmp     0x10, D0
                bne     loc_80000447
                mov     4, D0
                bra     loc_80000450
# ---------------------------------------------------------------------------

loc_80000447:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+93↑j
                and     0x40, D1
                cmp     0x40, D1
                bne     loc_80000463
                mov     6, D0

loc_80000450:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+64↑j
                                        # SCSI_4A_GetEventStatusNotification+8B↑j ...
                movbu   D0, (0x80010042)
                clr     D0
                movbu   D0, (0x80010044)

loc_8000045D:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+7F↑j
                movbu   D0, (0x80010045)

loc_80000463:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+9E↑j
                clr     D0
                mov     byte_80010040, D1
                exthu   D2
                mov     D2, A0
                call    ATA_Transfer, [D2,D3], 0x10
                extbu   D0
                cmp     1, D0
                bne     loc_8000047E
                call    ATA_SuccessRes, [], 0
                bra     locret_80000485
# ---------------------------------------------------------------------------

loc_8000047E:                           # CODE XREF: SCSI_4A_GetEventStatusNotification+C7↑j
                mov     4, D0  # IDNF
                call    ATA_ErrorRes, [], 0

locret_80000485:                        # CODE XREF: SCSI_4A_GetEventStatusNotification+CE↑j
                ret     [D2], 8
# End of function SCSI_4A_GetEventStatusNotification


# =============== S U B R O U T I N E =======================================


ATA_SuccessRes:                         # CODE XREF: SCSI_3_RequestSense+A↑p
                                        # SCSI_3_RequestSense+48↑p ...
                clr     D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x50, D0  # DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)
                rets
# End of function ATA_SuccessRes


# =============== S U B R O U T I N E =======================================


ATA_ErrorRes:                           # CODE XREF: PossiblyRaiseATAError+1A↑p
                                        # SCSI_Unhandled+19↑p ...
                movbu   D0, (ATA_taskfile_error)
                mov     1, D0
                movbu   D0, (ATA_taskfile_status)
                movbu   D0, (ATA_InterruptFlag)
                rets
# End of function ATA_ErrorRes


# =============== S U B R O U T I N E =======================================


Do_ATA_SuccessRes:                      # CODE XREF: Cmd55+102↑p
                call    ATA_SuccessRes, [], 0
                ret     [], 4
# End of function Do_ATA_SuccessRes


# =============== S U B R O U T I N E =======================================


SCSI_5A_ModeSense:                      # CODE XREF: SCSI_Dispatch+91↑p
                movbu   (CDB_in_5), D0
                cmp     8, D0
                beq     loc_800004C3
                cmp     9, D0
                beq     loc_80000523
                jmp     loc_8000054E
# ---------------------------------------------------------------------------

loc_800004C3:                           # CODE XREF: SCSI_5A_ModeSense+8↑j
                mov     0x10, D0
                movbu   D0, (0x80010031)
                movbu   (g_CDB2_From_SCSI_3B), D0
                mov     unk_80010030, D1
                movbu   D0, (0x80010033)
                movbu   (CDB_in_8), D0
                mov     D0, A0
                clr     D0
                call    ATA_Transfer, [D2,D3], 0x10
                extbu   D0
                cmp     1, D0
                bne     loc_800004F7
                call    ATA_SuccessRes, [], 0
                bra     loc_800004FE
# ---------------------------------------------------------------------------

loc_800004F7:                           # CODE XREF: SCSI_5A_ModeSense+3C↑j
                mov     4, D0  # IDNF
                call    ATA_ErrorRes, [], 0

loc_800004FE:                           # CODE XREF: SCSI_5A_ModeSense+43↑j
                movbu   (g_bWantErase), D0
                cmp     0, D0
                bne     loc_8000051A
                movbu   (g_CDB2_From_SCSI_3B), D0
                cmp     0, D0
                mov     1, D0
                movbu   D0, (g_bAllowErase)
                beq     locret_80000555

loc_8000051A:                           # CODE XREF: SCSI_5A_ModeSense+54↑j
                clr     D0
                movbu   D0, (g_bAllowErase)
                bra     locret_80000555
# ---------------------------------------------------------------------------

loc_80000523:                           # CODE XREF: SCSI_5A_ModeSense+C↑j
                call    ATA_SuccessRes, [], 0
                clr     D0
                bra     loc_8000052D
# ---------------------------------------------------------------------------

loc_8000052B:                           # CODE XREF: SCSI_5A_ModeSense+87↓j
                nop
                inc     D0

loc_8000052D:                           # CODE XREF: SCSI_5A_ModeSense+77↑j
                mov     D0, (g_DelayStore)
                cmp     1600000, D0
                bcs     loc_8000052B
                clr     D0
                call    IOConf1, [], 8
                clr     D0
                call    IOConf0, [], 0
                bset_s  1, byte_D900
                bra     locret_80000555
# ---------------------------------------------------------------------------

loc_8000054E:                           # CODE XREF: SCSI_5A_ModeSense+E↑j
                mov     4, D0  # IDNF
                call    ATA_ErrorRes, [], 0

locret_80000555:                        # CODE XREF: SCSI_5A_ModeSense+66↑j
                                        # SCSI_5A_ModeSense+6F↑j ...
                ret     [], 4
# End of function SCSI_5A_ModeSense


# =============== S U B R O U T I N E =======================================


SCSI_3B_WriteBuffer:                    # CODE XREF: SCSI_Dispatch+80↑p
                movbu   (g_bWantErase), D0
                cmp     0, D0
                bne     loc_800005B7
                movbu   (CDB_in_2), D0
                movbu   D0, (g_CDB2_From_SCSI_3B)
                movbu   (CDB_in_7), D2
                movbu   (CDB_in_8), D0
                asl     8, D2
                or      D0, D2
                bra     loc_8000059C
# ---------------------------------------------------------------------------

loc_80000581:                           # CODE XREF: SCSI_3B_WriteBuffer+46↓j
                mov     1, D0           # host to device
                mov     buf_80002000_flash, D1 # dst
                mov     0x800, A0       # count
                call    ATA_Transfer, [D2,D3], 0x10
                mov     D0, D1
                extbu   D1
                cmp     0, D1
                beq     loc_800005A0
                mov     0x800, D1
                sub     D1, D2

loc_8000059C:                           # CODE XREF: SCSI_3B_WriteBuffer+27↑j
                cmp     0, D2
                bne     loc_80000581

loc_800005A0:                           # CODE XREF: SCSI_3B_WriteBuffer+3D↑j
                extbu   D0
                cmp     1, D0
                bne     loc_800005AD
                call    ATA_SuccessRes, [], 0
                ret     [D2,D3,A2], 0x10
# ---------------------------------------------------------------------------

loc_800005AD:                           # CODE XREF: SCSI_3B_WriteBuffer+4B↑j
                mov     4, D0  # IDNF
                call    ATA_ErrorRes, [], 0
                ret     [D2,D3,A2], 0x10
# ---------------------------------------------------------------------------

loc_800005B7:                           # CODE XREF: SCSI_3B_WriteBuffer+8↑j
                movbu   (g_bReallyWantErase), D0
                cmp     1, D0
                bne     loc_8000062F
                call    EraseChipOrSector, [D2,D3,A2,A3], 0x14
                movbu   (g_FlashVendorID), D0
                mov     ROMMap_Atmel, D1
                mov     D1, (g_pROMMap)
                cmp     0x1F, D0  # Atmel
                beq     loc_80000610
                mov     ROMMap_Macronix, D1
                mov     D1, (g_pROMMap)
                cmp     0xC2, D0  # Macronix
                beq     loc_80000610
                cmp     0x37, D0  # AMIC
                beq     loc_80000610
                mov     ROMMap_SST, D1
                mov     D1, (g_pROMMap)
                cmp     0xBF, D0  # SST
                beq     loc_80000610
                mov     ROMMap_PMC, D1
                mov     D1, (g_pROMMap)

loc_80000610:                           # CODE XREF: SCSI_3B_WriteBuffer+82↑j
                                        # SCSI_3B_WriteBuffer+94↑j ...
                movbu   (g_EntrypointCmdMode), D0
                cmp     0, D0
                beq     loc_80000622
                add     0xC, D1
                mov     D1, (g_pROMMap)

loc_80000622:                           # CODE XREF: SCSI_3B_WriteBuffer+C0↑j
                clr     D0
                mov     D0, (g_dwCurFlashOffset) # set dst to 0
                movbu   D0, (g_bReallyWantErase)

loc_8000062F:                           # CODE XREF: SCSI_3B_WriteBuffer+67↑j
                movbu   (CDB_in_2), D0
                mov     0x800, A2
                movbu   D0, (g_CDB2_From_SCSI_3B)
                movbu   (CDB_in_7), D2
                movbu   (CDB_in_8), D0
                asl     8, D2
                mov     D2, D3
                or      D0, D3
                bra     flash_loop
# ---------------------------------------------------------------------------

flash_it:                               # CODE XREF: SCSI_3B_WriteBuffer+156↓j
                mov     1, D0           # host to device
                mov     buf_80002000_flash, D1 # dst
                mov     A2, A0          # count
                call    ATA_Transfer, [D2,D3], 0x10
                mov     D0, D2
                mov     D2, D1
                extbu   D1
                cmp     0, D1
                beq     set_error
                mov     (g_dwCurFlashOffset), D0
                movbu   (g_FlashTypeID_Always2), D1
                call    LooksLikeROMMapSizeCheck, [D2], 4
                extbu   D0
                cmp     0, D0
                beq     loc_8000069C
                movbu   (g_FlashAddressingType), D0 # flash type
                mov     (g_dwCurFlashOffset), D1 # dst offset (without ROM base)
                mov     buf_80002000_flash, A0 # src
                mov     A2, A1          # count
                call    ProgramFlash, [D2,D3,A2,A3], 0x18
                mov     D0, D2
                mov     D2, D1
                extbu   D1
                cmp     0, D1
                beq     set_error

loc_8000069C:                           # CODE XREF: SCSI_3B_WriteBuffer+123↑j
                mov     (g_dwCurFlashOffset), D1
                sub     A2, D3
                add     A2, D1
                mov     D1, (g_dwCurFlashOffset)

flash_loop:                             # CODE XREF: SCSI_3B_WriteBuffer+F8↑j
                cmp     0, D3
                bne     flash_it

set_error:                              # CODE XREF: SCSI_3B_WriteBuffer+10D↑j
                                        # SCSI_3B_WriteBuffer+142↑j
                extbu   D2
                cmp     0, D2
                bne     loc_800006CC
                mov     0xE4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

loc_800006CA:                           # CODE XREF: SCSI_3B_WriteBuffer:loc_800006CA↓j
                bra     loc_800006CA
# ---------------------------------------------------------------------------

loc_800006CC:                           # CODE XREF: SCSI_3B_WriteBuffer+15B↑j
                movbu   (g_CDB2_From_SCSI_3B), D0
                cmp     0, D0
                beq     loc_800006D9
                jmp     loc_80000759
# ---------------------------------------------------------------------------

loc_800006D9:                           # CODE XREF: SCSI_3B_WriteBuffer+17C↑j
                movbu   (g_FlashVendorID), D0
                mov     ROMMap_Atmel_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0x1F, D0  # Atmel
                beq     loc_80000723
                mov     ROMMap_Macronix_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0xC2, D0  # Macronix
                beq     loc_80000723
                cmp     0x37, D0  # AMIC
                beq     loc_80000723
                mov     ROMMap_SST_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0xBF, D0  # SST
                beq     loc_80000723
                mov     ROMMap_PMC_4000, A0
                mov     A0, (g_pROMMap)

loc_80000723:                           # CODE XREF: SCSI_3B_WriteBuffer+195↑j
                                        # SCSI_3B_WriteBuffer+1A7↑j ...
                mov     (A0), D1
                movbu   (8,A0), D2
                movbu   (g_FlashAddressingType), D0
                mov     unk_8000B000, A0
                asl     0xA, D2
                mov     D2, A1
                call    EraseSectorAndProgram, [D2,D3,A2,A3], 0x18
                extbu   D0
                cmp     0, D0
                bne     loc_80000759
                # ERROR EraseSectorAndProgram failed
                mov     0xF4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

SCSI_3B_InfLoop:                        # CODE XREF: SCSI_3B_WriteBuffer:loc_80000757↓j
                bra     SCSI_3B_InfLoop
# ---------------------------------------------------------------------------

loc_80000759:                           # CODE XREF: SCSI_3B_WriteBuffer+17E↑j
                                        # SCSI_3B_WriteBuffer+1E8↑j
                call    ATA_SuccessRes, [], 0
                ret     [D2,D3,A2], 0x10
# End of function SCSI_3B_WriteBuffer


# =============== S U B R O U T I N E =======================================


CmdNot55:                               # CODE XREF: Entrypoint:loc_8000004B↑p
                mov     1, D0
                call    IOConf0, [], 0
                mov     1, D0
                call    IOConf1, [], 8
                mov     g_FlashVendorID, D0
                mov     D0, (4,SP)
                mov     g_FlashDeviceID, D0
                mov     D0, (8,SP)
                mov     g_FlashTypeCol3, A0
                mov     g_FlashTypeID_Always2, A1
                mov     g_FlashTypeCol4, D0
                mov     g_FlashAddressingType, D1
                call    CheckFlashType, [D2,D3,A2,A3], 0x2C
                extbu   D0
                mov     D0, A3
                cmp     0xFF, A3
                bne     loc_800007CC
                # ERROR CheckFlashType failed
                mov     0xC4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                movbu   (g_FlashDeviceID), D0
                movbu   D0, (ATA_taskfile_bytecount_lo)
                movbu   (g_FlashVendorID), D0
                movbu   D0, (ATA_taskfile_bytecount_hi)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

loc_800007CA:                           # CODE XREF: CmdNot55:loc_800007CA↓j
                bra     loc_800007CA
# ---------------------------------------------------------------------------

loc_800007CC:                           # CODE XREF: CmdNot55+40↑j
                movbu   (g_FlashTypeID_Always2), D0
                call    SetD9A0, [], 0
                movbu   (g_FlashVendorID), D0
                mov     ROMMap_Atmel_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0x1F, D0
                beq     loc_80000821
                mov     ROMMap_Macronix_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0xC2, D0
                beq     loc_80000821
                cmp     0x37, D0 # '7'
                beq     loc_80000821
                mov     ROMMap_SST_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0xBF, D0
                beq     loc_80000821
                mov     ROMMap_PMC_4000, A0
                mov     A0, (g_pROMMap)

loc_80000821:                           # CODE XREF: CmdNot55+8A↑j CmdNot55+9C↑j ...
                movbu   (8,A0), D3
                mov     (A0), D1
                asl     0xA, D3
                mov     D3, A0
                mov     unk_8000B000, D0
                call    SetATABytecount, [A2], 4
                movbu   (g_EntrypointCmdMode), D0
                cmp     0, D0
                bne     loc_80000872
                movbu   (g_FlashTypeCol4), D0
                movbu   (g_FlashTypeID_Always2), D1
                call    EraseChip, [D2,D3,A2,A3], 0x18
                mov     D0, D2
                extbu   D2
                mov     D2, A3
                cmp     0, A3
                beq     EraseChipFailed
                jmp     loc_80000946
# ---------------------------------------------------------------------------

EraseChipFailed:                        # CODE XREF: CmdNot55+F5↑j
                mov     0xD4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

CmdNot55_ErrorLoop:                     # CODE XREF: CmdNot55:loc_80000870↓j
                bra     CmdNot55_ErrorLoop
# ---------------------------------------------------------------------------

loc_80000872:                           # CODE XREF: CmdNot55+DC↑j
                movbu   (g_FlashVendorID), D0
                mov     ROMMap_Atmel, A0
                mov     A0, (g_pROMMap)
                cmp     0x1F, D0
                beq     loc_800008BC
                mov     ROMMap_Macronix, A0
                mov     A0, (g_pROMMap)
                cmp     0xC2, D0
                beq     loc_800008BC
                cmp     0x37, D0
                beq     loc_800008BC
                mov     ROMMap_SST, A0
                mov     A0, (g_pROMMap)
                cmp     0xBF, D0
                beq     loc_800008BC
                mov     ROMMap_PMC, A0
                mov     A0, (g_pROMMap)

loc_800008BC:                           # CODE XREF: CmdNot55+125↑j
                                        # CmdNot55+137↑j ...
                cmp     0xBF, D0
                beq     loc_800008C8
                cmp     0x9D, D0
                bne     loc_800008CF

loc_800008C8:                           # CODE XREF: CmdNot55+15F↑j
                mov     0x6000, D3
                mov     4, D2
                bra     loc_80000942
# ---------------------------------------------------------------------------

loc_800008CF:                           # CODE XREF: CmdNot55+165↑j
                add     0xC, A0
                mov     A0, (g_pROMMap)
                mov     (A0), D3
                movbu   (8,A0), D2
                bra     loc_80000942
# ---------------------------------------------------------------------------

loc_800008DD:                           # CODE XREF: CmdNot55+1E3↓j
                movbu   (g_FlashTypeCol3), D0
                mov     D3, D1
                extbu   D2
                mov     D2, A0
                call    EraseSector, [D2,D3,A2,A3], 0x18 # D1 = Sector
                extbu   D0
                mov     D0, A3
                cmp     0, A3
                bne     loc_8000090A
                # ERROR EraseSector failed
                mov     0xD4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

CmdNot55_ErrorLoop2:                    # CODE XREF: CmdNot55:loc_80000908↓j
                bra     CmdNot55_ErrorLoop2
# ---------------------------------------------------------------------------

loc_8000090A:                           # CODE XREF: CmdNot55+190↑j
                movbu   (g_FlashVendorID), D0
                cmp     0xBF, D0
                beq     loc_8000091C
                cmp     0x9D, D0
                bne     loc_8000092E

loc_8000091C:                           # CODE XREF: CmdNot55+1B3↑j
                add     0x1000, D3
                mov     4, D2
                cmp     0x40000, D3
                bne     loc_80000942
                .byte 0x8F, 0xFF # mov     0xFF, D3
                bra     loc_80000942
# ---------------------------------------------------------------------------

loc_8000092E:                           # CODE XREF: CmdNot55+1B9↑j
                mov     (g_pROMMap), D0
                mov     0xC, A0
                add     D0, A0
                mov     A0, (g_pROMMap)
                mov     (A0), D3
                movbu   (8,A0), D2

loc_80000942:                           # CODE XREF: CmdNot55+16C↑j
                                        # CmdNot55+17A↑j ...
                .byte 0xAF, 0xFF # cmp     0xFF, D3
                bne     loc_800008DD

loc_80000946:                           # CODE XREF: CmdNot55+F7↑j
                movbu   (g_FlashVendorID), D0
                mov     ROMMap_Atmel, D1
                mov     D1, (g_pROMMap)
                cmp     0x1F, D0
                beq     loc_80000990
                mov     ROMMap_Macronix, D1
                mov     D1, (g_pROMMap)
                cmp     0xC2, D0
                beq     loc_80000990
                cmp     0x37, D0
                beq     loc_80000990
                mov     ROMMap_SST, D1
                mov     D1, (g_pROMMap)
                cmp     0xBF, D0
                beq     loc_80000990
                mov     ROMMap_PMC, D1
                mov     D1, (g_pROMMap)

loc_80000990:                           # CODE XREF: CmdNot55+1F9↑j
                                        # CmdNot55+20B↑j ...
                movbu   (g_EntrypointCmdMode), D0
                cmp     0, D0
                beq     loc_800009A2
                add     0xC, D1
                mov     D1, (g_pROMMap)

loc_800009A2:                           # CODE XREF: CmdNot55+237↑j
                clr     D0
                mov     D0, (g_dwCurFlashOffset)
                movbu   (g_FlashTypeID_Always2), D0
                mov     0x40000, D3
                cmp     2, D0
                beq     loc_800009C9
                mov     0x60000, D3
                cmp     4, D0
                beq     loc_800009C9
                mov     0x40000, D3

loc_800009C9:                           # CODE XREF: CmdNot55+256↑j
                                        # CmdNot55+260↑j
                mov     0x800, A2
                bra     loc_80000A28
# ---------------------------------------------------------------------------

loc_800009CE:                           # CODE XREF: CmdNot55+2C9↓j
                mov     1, D0
                mov     buf_80002000_flash, D1
                mov     A2, A0
                call    ATA_Transfer, [D2,D3], 0x10
                extbu   D0
                mov     D0, A3
                cmp     0, A3
                beq     loc_80000A2C
                mov     (g_dwCurFlashOffset), D0
                movbu   (g_FlashTypeID_Always2), D1
                call    LooksLikeROMMapSizeCheck, [D2], 4
                extbu   D0
                cmp     0, D0
                beq     loc_80000A18
                movbu   (g_FlashAddressingType), D0
                mov     (g_dwCurFlashOffset), D1
                mov     buf_80002000_flash, A0
                mov     A2, A1
                call    ProgramFlash, [D2,D3,A2,A3], 0x18
                extbu   D0
                mov     D0, A3
                cmp     0, A3
                beq     loc_80000A2C

loc_80000A18:                           # CODE XREF: CmdNot55+296↑j
                mov     (g_dwCurFlashOffset), D0
                sub     A2, D3
                add     A2, D0
                mov     D0, (g_dwCurFlashOffset)

loc_80000A28:                           # CODE XREF: CmdNot55+26B↑j
                cmp     0, D3
                bne     loc_800009CE

loc_80000A2C:                           # CODE XREF: CmdNot55+280↑j
                                        # CmdNot55+2B5↑j
                cmp     0, A3
                bne     loc_80000A47
                # ERROR Transfer or Program failed
                mov     0xE4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

CmdNot55_ErrorLoop3:                    # CODE XREF: CmdNot55:loc_80000A45↓j
                bra     CmdNot55_ErrorLoop3
# ---------------------------------------------------------------------------

loc_80000A47:                           # CODE XREF: CmdNot55+2CD↑j
                movbu   (g_FlashVendorID), D0
                mov     ROMMap_Atmel_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0x1F, D0
                beq     loc_80000A91
                mov     ROMMap_Macronix_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0xC2, D0
                beq     loc_80000A91
                cmp     0x37, D0
                beq     loc_80000A91
                mov     ROMMap_SST_4000, A0
                mov     A0, (g_pROMMap)
                cmp     0xBF, D0
                beq     loc_80000A91
                mov     ROMMap_PMC_4000, A0
                mov     A0, (g_pROMMap)

loc_80000A91:                           # CODE XREF: CmdNot55+2FA↑j
                                        # CmdNot55+30C↑j ...
                mov     (A0), D1
                movbu   (8,A0), D2
                movbu   (g_FlashAddressingType), D0
                mov     unk_8000B000, A0
                asl     0xA, D2
                mov     D2, A1
                call    EraseForSomeVendorsAndProgram, [D2,D3,A2,A3], 0x18
                extbu   D0
                cmp     0, D0
                bne     loc_80000AC7
                mov     0xF4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

CmdNot55_ErrorLoop4:                    # CODE XREF: CmdNot55:loc_80000AC5↓j
                bra     CmdNot55_ErrorLoop4
# ---------------------------------------------------------------------------

loc_80000AC7:                           # CODE XREF: CmdNot55+34D↑j
                clr     D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x50, D0  # DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)
                clr     D0
                bra     loc_80000ADF
# ---------------------------------------------------------------------------

loc_80000ADD:                           # CODE XREF: CmdNot55+38A↓j
                nop
                inc     D0

loc_80000ADF:                           # CODE XREF: CmdNot55+37A↑j
                mov     D0, (g_DelayStore)
                cmp     3200000, D0
                bcs     loc_80000ADD
                clr     D0
                call    IOConf1, [], 8
                clr     D0
                call    IOConf0, [], 0
                bset_s  1, byte_D900
                ret     [D2,D3,A2,A3], 0x1C
# End of function CmdNot55


# =============== S U B R O U T I N E =======================================


IOConf0:                                # CODE XREF: Cmd55+2↑p
                                        # SCSI_5A_ModeSense+90↑p ...
                extbu   D0
                cmp     0, D0
                bne     loc_80000B0D
                bclr_s  1, byte_D9A8
                bra     locret_80000B12
# ---------------------------------------------------------------------------

loc_80000B0D:                           # CODE XREF: IOConf0+3↑j
                bset_s  1, byte_D9A8

locret_80000B12:                        # CODE XREF: IOConf0+A↑j
                rets
# End of function IOConf0


# =============== S U B R O U T I N E =======================================


SetD9A0:                                # CODE XREF: Cmd55+5F↑p CmdNot55+71↑p
                extbu   D0
                cmp     2, D0
                beq     loc_80000B1F
                cmp     4, D0
                beq     loc_80000B26
                bra     loc_80000B2D
# ---------------------------------------------------------------------------

loc_80000B1F:                           # CODE XREF: SetD9A0+3↑j
                mov     0x14, D0
                movbu   D0, (byte_D9A0)
                bra     locret_80000B32
# ---------------------------------------------------------------------------

loc_80000B26:                           # CODE XREF: SetD9A0+7↑j
                mov     0x24, D0
                movbu   D0, (byte_D9A0)
                bra     locret_80000B32
# ---------------------------------------------------------------------------

loc_80000B2D:                           # CODE XREF: SetD9A0+9↑j
                mov     0x14, D0
                movbu   D0, (byte_D9A0)

locret_80000B32:                        # CODE XREF: SetD9A0+10↑j SetD9A0+17↑j
                rets
# End of function SetD9A0


# =============== S U B R O U T I N E =======================================


CheckFlashType:                         # CODE XREF: Cmd55+36↑p CmdNot55+36↑p
                mov     D1, (4,SP)
                mov     D0, (8,SP)
                mov     A0, (0x10,SP)
                mov     A1, (0xC,SP)
                mov     TICKLE_5555, A3
                mov     0xAA, D0
                movbu   D0, (A3)        # write
                mov     0x55, D0
                movbu   D0, (TICKLE_2AAA) # write
                mov     0x90, D0
                movbu   D0, (A3)        # write
                nop
                nop
                movbu   (0x90000000), D0 # read
                movbu   D0, (0x18,SP)
                movbu   (0x90000100), D0 # read
                movbu   (0x18,SP), D1   # D1 = byte(90000000)
                movbu   D0, (0x19,SP)   # var_19 = byte(90000100)
                movbu   (0x90000200), D0 # read
                movbu   D0, (0x1A,SP)
                movbu   (0x19,SP), D0
                cmp     D0, D1          # byte(90000000) == byte(90000100)?
                movbu   (0x90000001), D0 # read
                movbu   D0, (0x14,SP)
                bne     loc_80000BF2
                movbu   (0x19,SP), D1
                movbu   (0x1A,SP), D0
                cmp     D0, D1
                bne     loc_80000BF2
                mov     (0x30,SP), A0
                movbu   (0x18,SP), D0
                clr     D1
                movbu   D0, (A0)
                movbu   (0x14,SP), D0
                mov     (0x34,SP), A0
                movbu   D0, (A0)
                bra     loc_80000BEA
# ---------------------------------------------------------------------------

loc_80000B9D:                           # CODE XREF: CheckFlashType+B2↓j
                                        # CheckFlashType+BC↓j
                mov     D1, D0
                extbu   D0
                mov     A1, A0
                mov     6, D2
                mulu    D2, D0
                add     D0, A0
                movbu   (A0), D2
                movbu   (0x18,SP), D0
                cmp     D0, D2
                bne     loc_80000BD8
                movbu   (1,A0), D2
                movbu   (0x14,SP), D0
                cmp     D0, D2
                bne     loc_80000BD8
                mov     (0xC,SP), A1
                movbu   (2,A0), D0
                movbu   D0, (A1)
                movbu   (3,A0), D0
                mov     (0x10,SP), A1
                movbu   D0, (A1)
                movbu   (4,A0), D0
                mov     (8,SP), A1
                movbu   D0, (A1)
                movbu   (5,A0), D0
                mov     (4,SP), A0
                movbu   D0, (A0)
                movbu   (0x18,SP), D2
                bra     loc_80000BF5
# ---------------------------------------------------------------------------

loc_80000BD8:                           # CODE XREF: CheckFlashType+78↑j
                                        # CheckFlashType+81↑j
                mov     D1, D0
                extbu   D0
                mov     6, D2
                mulu    D2, D0
                movbu   (D0,A1), D0
                extbu   D1
                inc     D1
                cmp     0xFF, D0
                bne     loc_80000B9D
                bra     loc_80000BF2
# ---------------------------------------------------------------------------

loc_80000BEA:                           # CODE XREF: CheckFlashType+67↑j
                mov     g_FlashTable_0, A1
                bra     loc_80000B9D
# ---------------------------------------------------------------------------

loc_80000BF2:                           # CODE XREF: CheckFlashType+4D↑j
                                        # CheckFlashType+56↑j ...
                mov     0xFF, D2

loc_80000BF5:                           # CODE XREF: CheckFlashType+A2↑j
                mov     0xAA, D0
                movbu   D0, (A3)        # write 90005555
                mov     0x55, D0
                movbu   D0, (TICKLE_2AAA)
                mov     0xF0, D0
                movbu   D0, (A3)
                call    DebounceFirstROMByte, [], 4
                nop
                nop
                mov     0xFF, D3
                mov     D2, D0
                extbu   D0
                cmp     D3, D0          # 0xFF == extbu(D2)
                beq     loc_80000C19    # assume taken
                jmp     Done
# ---------------------------------------------------------------------------

loc_80000C19:                           # CODE XREF: CheckFlashType+E0↑j
                mov     TICKLE_555, A2
                mov     0xAA, D0
                movbu   D0, (A2)        # write 90000555
                mov     0x55, D0
                movbu   D0, (TICKLE_2AA) # write 900002AA
                mov     0x90, D0
                movbu   D0, (A2)        # write 90000555
                nop
                nop
                movbu   (0x90000000), D0
                movbu   D0, (0x18,SP)
                movbu   (0x90000100), D0
                movbu   (0x18,SP), D1
                movbu   D0, (0x19,SP)
                movbu   (0x90000200), D0
                movbu   D0, (0x1A,SP)
                movbu   (0x19,SP), D0
                cmp     D0, D1
                movbu   (0x90000001), D0
                movbu   D0, (0x14,SP)
                beq     loc_80000C63    # assume not taken
                jmp     loc_80000D2D
# ---------------------------------------------------------------------------

loc_80000C63:                           # CODE XREF: CheckFlashType+12A↑j
                movbu   (0x19,SP), D1
                movbu   (0x1A,SP), D0
                cmp     D0, D1
                beq     loc_80000C6F
                jmp     loc_80000D2D
# ---------------------------------------------------------------------------

loc_80000C6F:                           # CODE XREF: CheckFlashType+136↑j
                mov     (0x30,SP), A0
                movbu   (0x18,SP), D0
                clr     D1
                movbu   D0, (A0)
                movbu   (0x14,SP), D0
                mov     (0x34,SP), A0
                movbu   D0, (A0)
                bra     loc_80000CCB
# ---------------------------------------------------------------------------

loc_80000C80:                           # CODE XREF: CheckFlashType+192↓j
                                        # CheckFlashType+19D↓j
                mov     D1, D0
                extbu   D0
                mov     A1, A0
                mov     6, D2
                mulu    D2, D0
                add     D0, A0
                movbu   (A0), D2
                movbu   (0x18,SP), D0
                cmp     D0, D2
                bne     loc_80000CBB
                movbu   (1,A0), D2
                movbu   (0x14,SP), D0
                cmp     D0, D2
                bne     loc_80000CBB
                mov     (0xC,SP), A1
                movbu   (2,A0), D0
                movbu   D0, (A1)
                movbu   (3,A0), D0
                mov     (0x10,SP), A1
                movbu   D0, (A1)
                movbu   (4,A0), D0
                mov     (8,SP), A1
                movbu   D0, (A1)
                movbu   (5,A0), D0
                mov     (4,SP), A0
                movbu   D0, (A0)
                movbu   (0x18,SP), D2
                bra     loc_80000CD3
# ---------------------------------------------------------------------------

loc_80000CBB:                           # CODE XREF: CheckFlashType+15B↑j
                                        # CheckFlashType+164↑j
                mov     D1, D0
                extbu   D0
                mov     6, D2
                mulu    D2, D0
                movbu   (D0,A1), D0
                extbu   D1
                inc     D1
                cmp     D3, D0
                bne     loc_80000C80
                mov     D3, D2
                bra     loc_80000CD3
# ---------------------------------------------------------------------------

loc_80000CCB:                           # CODE XREF: CheckFlashType+14A↑j
                mov     g_FlashTable_1, A1
                bra     loc_80000C80
# ---------------------------------------------------------------------------

loc_80000CD3:                           # CODE XREF: CheckFlashType+185↑j
                                        # CheckFlashType+195↑j
                mov     D2, D0
                extbu   D0
                cmp     D3, D0
                bne     loc_80000D2E
                clr     D1
                bra     loc_80000D25
# ---------------------------------------------------------------------------

loc_80000CDB:                           # CODE XREF: CheckFlashType+1ED↓j
                                        # CheckFlashType+1F7↓j
                mov     D1, D0
                extbu   D0
                mov     A1, A0
                mov     6, D2
                mulu    D2, D0
                add     D0, A0
                movbu   (A0), D2
                movbu   (0x18,SP), D0
                cmp     D0, D2
                bne     loc_80000D16
                movbu   (1,A0), D2
                movbu   (0x14,SP), D0
                cmp     D0, D2
                bne     loc_80000D16
                mov     (0xC,SP), A1
                movbu   (2,A0), D0
                movbu   D0, (A1)
                movbu   (3,A0), D0
                mov     (0x10,SP), A1
                movbu   D0, (A1)
                movbu   (4,A0), D0
                mov     (8,SP), A1
                movbu   D0, (A1)
                movbu   (5,A0), D0
                mov     (4,SP), A0
                movbu   D0, (A0)
                movbu   (0x18,SP), D2
                bra     loc_80000D2E
# ---------------------------------------------------------------------------

loc_80000D16:                           # CODE XREF: CheckFlashType+1B6↑j
                                        # CheckFlashType+1BF↑j
                mov     D1, D0
                extbu   D0
                mov     6, D2
                mulu    D2, D0
                movbu   (D0,A1), D0
                extbu   D1
                inc     D1
                cmp     D3, D0
                bne     loc_80000CDB
                bra     loc_80000D2D
# ---------------------------------------------------------------------------

loc_80000D25:                           # CODE XREF: CheckFlashType+1A5↑j
                mov     g_FlashTable_3, A1
                bra     loc_80000CDB
# ---------------------------------------------------------------------------

loc_80000D2D:                           # CODE XREF: CheckFlashType+12C↑j
                                        # CheckFlashType+138↑j ...
                mov     D3, D2

loc_80000D2E:                           # CODE XREF: CheckFlashType+1A2↑j
                                        # CheckFlashType+1E0↑j
                mov     0xAA, D0
                movbu   D0, (A2)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AA)
                mov     0xF0, D0
                movbu   D0, (A2)
                call    DebounceFirstROMByte, [], 4
                nop
                nop
                mov     D2, D0
                extbu   D0
                cmp     D3, D0
                beq     loc_80000D4F    # assume taken
                jmp     Done
# ---------------------------------------------------------------------------

loc_80000D4F:                           # CODE XREF: CheckFlashType+216↑j
                mov     0xAA, D0
                movbu   D0, (A3)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AAA)
                mov     0x90, D0
                movbu   D0, (A3)
                nop
                nop
                movbu   (0x90000000), D0
                movbu   D0, (0x18,SP)
                movbu   (0x90000000), D0
                movbu   (0x18,SP), D1
                movbu   D0, (0x19,SP)
                movbu   (0x90000000), D0
                movbu   D0, (0x1A,SP)
                movbu   (0x19,SP), D0
                cmp     D0, D1
                movbu   (0x90000001), D0
                movbu   D0, (0x14,SP)
                bne     loc_80000DFC    # assume not taken
                movbu   (0x19,SP), D1
                movbu   (0x1A,SP), D0
                cmp     D0, D1
                bne     loc_80000DFC    # assume not taken
                mov     (0x30,SP), A0   # A0 = g_ROMMapSelector
                movbu   (0x18,SP), D0
                movbu   D0, (A0)        # (A0) = (byte_90000000)
                movbu   (0x14,SP), D0
                mov     (0x34,SP), A0   # A0 = byte_80001830
                movbu   D0, (A0)        # (A0) = (byte_90000001)
                clr     D0
                bra     SetEntryList
# ---------------------------------------------------------------------------

CheckEntry:                             # CODE XREF: CheckFlashType+2BC↓j
                                        # CheckFlashType+2C6↓j
                mov     D0, D1
                extbu   D1
                mov     A1, A0
                mov     6, D2
                mulu    D2, D1
                add     D1, A0
                movbu   (A0), D2        # D2 = 0xBF
                movbu   (0x18,SP), D1
                cmp     D1, D2
                bne     NextEntry
                movbu   (1,A0), D2
                movbu   (0x14,SP), D1
                cmp     D1, D2
                bne     NextEntry
                mov     (0xC,SP), A1
                movbu   (2,A0), D0
                movbu   D0, (A1)
                movbu   (3,A0), D0
                mov     (0x10,SP), A1
                movbu   D0, (A1)
                movbu   (4,A0), D0
                mov     (8,SP), A1
                movbu   D0, (A1)
                movbu   (5,A0), D0
                mov     (4,SP), A0
                movbu   D0, (A0)
                movbu   (0x18,SP), D2
                bra     loc_80000DFD
# ---------------------------------------------------------------------------

NextEntry:                              # CODE XREF: CheckFlashType+285↑j
                                        # CheckFlashType+28E↑j
                mov     D0, D1
                extbu   D1
                mov     6, D2
                mulu    D2, D1
                movbu   (D1,A1), D1
                extbu   D0
                inc     D0
                cmp     D3, D1
                bne     CheckEntry
                bra     loc_80000DFC
# ---------------------------------------------------------------------------

SetEntryList:                           # CODE XREF: CheckFlashType+274↑j
                mov     g_FlashTable_2, A1
                bra     CheckEntry
# ---------------------------------------------------------------------------

loc_80000DFC:                           # CODE XREF: CheckFlashType+25A↑j
                                        # CheckFlashType+263↑j ...
                mov     D3, D2

loc_80000DFD:                           # CODE XREF: CheckFlashType+2AF↑j
                mov     0xAA, D0
                movbu   D0, (A3)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AAA)
                mov     0xF0, D0
                movbu   D0, (A3)
                call    DebounceFirstROMByte, [], 4
                nop
                nop

Done:                                   # CODE XREF: CheckFlashType+E2↑j
                                        # CheckFlashType+218↑j
                mov     D2, D0
                extbu   D0
                ret     [D2,D3,A2,A3], 0x2C
# End of function CheckFlashType


# =============== S U B R O U T I N E =======================================


EraseChip:                              # CODE XREF: CmdNot55+EA↑p
                                        # EraseChipOrSector+16↓p
                movbu   D0, (4,SP)
                mov     D1, D3
                clr     D2
                mov     TICKLE_555, A3
                mov     TICKLE_5555, A2
                extbu   D3
                asl     0x11, D3

loc_80000E30:                           # CODE XREF: EraseChip+8D↓j
                movbu   (4,SP), D0
                extbu   D2
                inc     D2
                cmp     0, D0
                beq     loc_80000E3F
                cmp     1, D0
                beq     loc_80000E64
                bra     loc_80000E8E
# ---------------------------------------------------------------------------

loc_80000E3F:                           # CODE XREF: EraseChip+1C↑j
                mov     0xAA, D0
                movbu   D0, (A3)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AA)
                mov     0x80, D0
                movbu   D0, (A3)
                mov     0xAA, D0
                movbu   D0, (A3)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AA)
                mov     0x10, D0
                movbu   D0, (A3)
                bra     loc_80000E87
# ---------------------------------------------------------------------------

loc_80000E64:                           # CODE XREF: EraseChip+20↑j
                mov     0xAA, D0
                movbu   D0, (A2)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AAA)
                mov     0x80, D0
                movbu   D0, (A2)
                mov     0xAA, D0
                movbu   D0, (A2)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AAA)
                mov     0x10, D0
                movbu   D0, (A2)

loc_80000E87:                           # CODE XREF: EraseChip+47↑j
                call    DebounceFirstROMByte, [], 4
                bra     loc_80000E91
# ---------------------------------------------------------------------------

loc_80000E8E:                           # CODE XREF: EraseChip+22↑j
                clr     D0
                bra     locret_80000EAD
# ---------------------------------------------------------------------------

loc_80000E91:                           # CODE XREF: EraseChip+71↑j
                mov     FLASH_BASE, D0
                mov     D3, D1
                call    CheckAllFF, [D2,D3], 8
                mov     1, D1
                extbu   D0
                cmp     0, D0
                bne     loc_80000EAB
                mov     D2, D0
                extbu   D0
                cmp     5, D0
                ble     loc_80000E30
                clr     D1

loc_80000EAB:                           # CODE XREF: EraseChip+87↑j
                mov     D1, D0
                extbu   D0

locret_80000EAD:                        # CODE XREF: EraseChip+74↑j
                ret     [D2,D3,A2,A3], 0x18
# End of function EraseChip


# =============== S U B R O U T I N E =======================================

# D1 = Sector

EraseSector:                            # CODE XREF: CmdNot55+186↑p
                                        # EraseForSomeVendorsAndProgram+1D↓p ...
                movbu   D0, (4,SP)
                mov     A0, D3
                clr     D2
                mov     FLASH_BASE, A3
                add     D1, A3
                extbu   D3
                asl     0xA, D3

loc_80000EC2:                           # CODE XREF: EraseSector+C9↓j
                movbu   (4,SP), D1
                extbu   D2
                inc     D2
                cmp     0, D1
                beq     loc_80000ED6
                cmp     4, D1
                beq     loc_80000F08
                cmp     5, D1
                beq     loc_80000F32
                jmp     loc_80000F61
# ---------------------------------------------------------------------------

loc_80000ED6:                           # CODE XREF: EraseSector+19↑j
                mov     0xAA, D0
                movbu   D0, (TICKLE_555)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AA)
                mov     0x80, D0
                movbu   D0, (TICKLE_555)
                mov     0xAA, D0
                movbu   D0, (TICKLE_555)
                mov     0x55, D0
                movbu   D0, (TICKLE_2AA)
                mov     A3, A0
                mov     0x30, D0
                movbu   D0, (A0)
                bra     loc_80000F5A
# ---------------------------------------------------------------------------

loc_80000F08:                           # CODE XREF: EraseSector+1D↑j
                mov     TICKLE_5555, A0
                mov     0xAA, D0
                movbu   D0, (A0)
                mov     TICKLE_2AAA, A1
                mov     0x55, D0
                movbu   D0, (A1)
                mov     0x80, D0
                movbu   D0, (A0)
                mov     0xAA, D0
                movbu   D0, (A0)
                mov     0x55, D0
                movbu   D0, (A1)
                mov     A3, A0
                mov     0x30, D0
                movbu   D0, (A0)
                bra     loc_80000F5A
# ---------------------------------------------------------------------------

loc_80000F32:                           # CODE XREF: EraseSector+21↑j
                mov     TICKLE_5555, A0
                mov     0xAA, D0
                movbu   D0, (A0)
                mov     TICKLE_2AAA, A1
                mov     0x55, D0
                movbu   D0, (A1)
                mov     0x80, D0
                movbu   D0, (A0)
                mov     0xAA, D0
                movbu   D0, (A0)
                mov     0x55, D0
                movbu   D0, (A1)
                mov     A3, A0
                mov     0x30, D0
                movbu   D0, (A0)

loc_80000F5A:                           # CODE XREF: EraseSector+56↑j
                                        # EraseSector+80↑j
                call    DebounceFirstROMByte, [], 4
                bra     loc_80000F64
# ---------------------------------------------------------------------------

loc_80000F61:                           # CODE XREF: EraseSector+23↑j
                clr     D0
                bra     locret_80000F7F
# ---------------------------------------------------------------------------

loc_80000F64:                           # CODE XREF: EraseSector+AF↑j
                mov     A3, D0
                mov     D3, D1
                call    CheckAllFF, [D2,D3], 8
                mov     1, D1
                extbu   D0
                cmp     0, D0
                bne     loc_80000F7D
                mov     D2, D1
                extbu   D1
                cmp     5, D1
                bgt     loc_80000F7C
                jmp     loc_80000EC2
# ---------------------------------------------------------------------------

loc_80000F7C:                           # CODE XREF: EraseSector+C7↑j
                clr     D1

loc_80000F7D:                           # CODE XREF: EraseSector+C1↑j
                mov     D1, D0
                extbu   D0

locret_80000F7F:                        # CODE XREF: EraseSector+B2↑j
                ret     [D2,D3,A2,A3], 0x18
# End of function EraseSector


# =============== S U B R O U T I N E =======================================


CheckAllFF:                             # CODE XREF: EraseChip+7D↑p
                                        # EraseSector+B7↑p
                mov     D0, A1
                mov     1, D0
                clr     D3
                bra     loc_80000F97
# ---------------------------------------------------------------------------

loc_80000F89:                           # CODE XREF: CheckAllFF+16↓j
                mov     A1, A0
                movbu   (A0), D2
                inc     A1
                cmp     0xFF, D2
                beq     loc_80000F96
                clr     D0
                bra     loc_80000F9A
# ---------------------------------------------------------------------------

loc_80000F96:                           # CODE XREF: CheckAllFF+F↑j
                inc     D3

loc_80000F97:                           # CODE XREF: CheckAllFF+5↑j
                cmp     D1, D3
                bcs     loc_80000F89

loc_80000F9A:                           # CODE XREF: CheckAllFF+12↑j
                extbu   D0
                retf    [D2,D3], 8
# End of function CheckAllFF


# =============== S U B R O U T I N E =======================================


ProgramFlash:                           # CODE XREF: SCSI_3B_WriteBuffer+138↑p
                                        # CmdNot55+2AB↑p ...
                mov     A1, A3
                movbu   D0, (4,SP)
                mov     A0, A2          # Src
                mov     D1, D3
                clr     D2
                add     FLASH_BASE, D3

loc_80000FAB:                           # CODE XREF: ProgramFlash+A3↓j
                movbu   (4,SP), D0
                extbu   D2
                inc     D2
                cmp     0, D0
                beq     FlashType0
                cmp     1, D0
                beq     FlashType1
                jmp     Fail
# ---------------------------------------------------------------------------

FlashType0:                             # CODE XREF: ProgramFlash+14↑j
                movbu   (A2), D0
                mov     0xAA, D1
                movbu   D1, (TICKLE_555)
                mov     0x55, D1
                movbu   D1, (TICKLE_2AA)
                mov     0xA0, D1
                movbu   D1, (TICKLE_555)
                mov     D3, A0
                movbu   D0, (A0)        # Program
                call    DebounceFirstROMByte, [], 4
                mov     D3, D0          # dst addr (what was just written)
                mov     A2, D1          # src addr (what it should be)
                mov     1, A0           # byte count
                call    VerifyBytes, [D2,D3,A2,A3], 0x14
                extbu   D0
                cmp     0, D0
                beq     loc_80001039    # Retry
                inc     D3              # Next byte (dst)
                inc     A2              # Next byte (src)
                clr     D2
                mov     1, D0
                sub     D0, A3
                bne     loc_80001039
                bra     locret_80001045
# ---------------------------------------------------------------------------

FlashType1:                             # CODE XREF: ProgramFlash+18↑j
                movbu   (A2), D0
                mov     0xAA, D1
                movbu   D1, (TICKLE_5555)
                mov     0x55, D1
                movbu   D1, (TICKLE_2AAA)
                mov     0xA0, D1
                movbu   D1, (TICKLE_5555)
                mov     D3, A0
                movbu   D0, (A0)
                call    DebounceFirstROMByte, [], 4
                mov     D3, D0
                mov     A2, D1
                mov     1, A0
                call    VerifyBytes, [D2,D3,A2,A3], 0x14
                extbu   D0
                cmp     0, D0
                beq     loc_80001039
                inc     D3
                inc     A2
                clr     D2
                mov     1, D0
                sub     D0, A3
                bne     loc_80001039
                bra     locret_80001045
# ---------------------------------------------------------------------------

loc_80001039:                           # CODE XREF: ProgramFlash+4F↑j
                                        # ProgramFlash+58↑j ...
                mov     D2, D0
                extbu   D0
                cmp     200, D0
                bgt     Fail
                jmp     loc_80000FAB
# ---------------------------------------------------------------------------

Fail:                                   # CODE XREF: ProgramFlash+1A↑j
                                        # ProgramFlash+A1↑j
                clr     D0

locret_80001045:                        # CODE XREF: ProgramFlash+5A↑j
                                        # ProgramFlash+99↑j
                ret     [D2,D3,A2,A3], 0x18
# End of function ProgramFlash


# =============== S U B R O U T I N E =======================================


VerifyBytes:                            # CODE XREF: ProgramFlash+47↑p
                                        # ProgramFlash+86↑p
                mov     1, D2
                movbu   D2, (SP)
                mov     D1, A3
                clr     D3
                bra     loc_80001063
# ---------------------------------------------------------------------------

loc_80001052:                           # CODE XREF: VerifyBytes+1D↓j
                mov     A3, A1
                mov     D0, A2
                movbu   (A2), D2
                movbu   (A1), D1
                inc     D0
                inc     A3
                cmp     D1, D2
                beq     loc_80001062
                clr     D1
                movbu   D1, (SP)

loc_80001062:                           # CODE XREF: VerifyBytes+14↑j
                inc     D3

loc_80001063:                           # CODE XREF: VerifyBytes+8↑j
                cmp     A0, D3
                bcs     loc_80001052
                movbu   (SP), D0
                retf    [D2,D3,A2,A3], 0x14
# End of function VerifyBytes

# ---------------------------------------------------------------------------

LooksLikeROMMapSizeCheck:               # CODE XREF: SCSI_3B_WriteBuffer+11B↑p
                                        # CmdNot55+28E↑p
                extbu   D1
                mov     0x40000, D2
                cmp     2, D1
                beq     loc_80001088
                mov     0x60000, D2
                cmp     4, D1
                beq     loc_80001088
                mov     0x40000, D2

loc_80001088:                           # CODE XREF: CACHE:80001076↑j
                                        # CACHE:80001080↑j
                cmp     D0, D2
                bls     loc_800010BB
                mov     (g_pROMMap), A0
                mov     (A0), D1
                mov     (4,A0), A0
                cmp     D0, A0
                bcc     loc_800010AD
                mov     (g_pROMMap), D1
                mov     0xC, A0
                add     D1, A0
                mov     A0, (g_pROMMap)
                mov     (A0), D1
                mov     (4,A0), A0

loc_800010AD:                           # CODE XREF: CACHE:80001097↑j
                cmp     D1, D0
                bcs     loc_800010BB
                cmp     D0, D1
                bhi     loc_800010BB
                cmp     A0, D0
                bhi     loc_800010BB
                mov     1, D0
                bra     locret_800010BC
# ---------------------------------------------------------------------------

loc_800010BB:                           # CODE XREF: CACHE:80001089↑j
                                        # CACHE:800010AE↑j ...
                clr     D0

locret_800010BC:                        # CODE XREF: CACHE:800010B9↑j
                retf    [D2], 4

# =============== S U B R O U T I N E =======================================


DebounceFirstROMByte:                   # CODE XREF: CheckFlashType+D3↑p
                                        # CheckFlashType+20C↑p ...
                nop
                nop
                setlb
                movbu   (FLASH_BASE), D0 # D0 = byte(90000000)
                movbu   D0, (SP)        # (SP) = D0
                movbu   (FLASH_BASE), D0 # D0 = byte(90000000)
                movbu   (SP), D1        # D1 = (SP)
                movbu   D0, (1,SP)      # (1,SP) = D0
                movbu   (1,SP), D0      # D0 = (1,SP)
                cmp     D0, D1
                movbu   (FLASH_BASE), D0
                movbu   D0, (2,SP)
                lne
                movbu   (1,SP), D1
                movbu   (2,SP), D0
                cmp     D0, D1
                lne
                ret     [], 4
# End of function DebounceFirstROMByte


# =============== S U B R O U T I N E =======================================


ATA_Transfer:                           # CODE XREF: SCSI_3_RequestSense+3E↑p
                                        # SCSI_4A_GetEventStatusNotification+BF↑p ...
                mov     A0, D2
                mov     2, D3
                movbu   D3, (g_ATATransferDir) # Transfer direction
                extbu   D0
                cmp     1, D0
                beq     loc_80001107
                mov     1, D0
                movbu   D0, (g_ATATransferDir)

loc_80001107:                           # CODE XREF: ATA_Transfer+D↑j
                inc     D2
                and     0xFFFE, D2
                movhu   D2, (g_ATATransferLen) # Transfer length
                mov     D1, (g_ATATransferPtr) # Dst Pointer
                call    ATA_Transfer_Subfunc, [A2], 8
                mov     0x80, D1
                setlb
                movbu   (byte_D950), D0
                and     D1, D0
                cmp     D1, D0
                lne
                bset_s  0x80, byte_D950
                setlb
                movbu   (byte_DFB0), D0
                and     0x20, D0
                cmp     0x20, D0
                lne
                bclr_s  0x20, byte_DFB0
                setlb
                movbu   (byte_D954), D0
                and     4, D0
                cmp     4, D0
                lne
                bset_s  4, byte_D954
                movbu   (byte_DF38), D0
                movbu   D0, (4,SP)
                movbu   (byte_DF39), D0
                movbu   D0, (4,SP)
                movbu   (byte_DF3A), D0
                movbu   D0, (4,SP)
                movbu   (byte_DF3B), D0
                movbu   D0, (4,SP)
                movbu   (byte_DF3C), D0
                movbu   D0, (4,SP)
                movbu   (byte_DF3D), D0
                movbu   D0, (4,SP)
                movbu   (byte_DF3E), D0
                movbu   D0, (4,SP)
                movbu   (byte_DF3F), D0
                movbu   D0, (4,SP)
                mov     1, D0
                ret     [D2,D3], 0x10
# End of function ATA_Transfer


# =============== S U B R O U T I N E =======================================


ATA_Transfer_Subfunc:                   # CODE XREF: ATA_Transfer+28↑p
                movbu   (ATA_DA04), D0
                mov     0xDA04, A0
                or      0x80, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                and     0xFFFFFF7F, D0
                movbu   D0, (A0)
                movbu   (g_ATATransferDir), D0
                cmp     2, D0
                bne     loc_800011B3
                mov     (g_ATATransferPtr), D0
                movhu   (g_ATATransferLen), D1
                call    TransferHostToDevice, [D2], 0x18
                bra     loc_800011C4
# ---------------------------------------------------------------------------

loc_800011B3:                           # CODE XREF: ATA_Transfer_Subfunc+1E↑j
                mov     (g_ATATransferPtr), D0
                movhu   (g_ATATransferLen), D1
                call    TransferDeviceToHost, [D2], 0x18

loc_800011C4:                           # CODE XREF: ATA_Transfer_Subfunc+31↑j
                movbu   (g_ATATransferDir), D0
                cmp     2, D0
                bne     loc_800011D4
                clr     D0
                movbu   D0, (ATA_taskfile_seccount)
                bra     loc_800011D9
# ---------------------------------------------------------------------------

loc_800011D4:                           # CODE XREF: ATA_Transfer_Subfunc+4C↑j
                mov     2, D0
                movbu   D0, (ATA_taskfile_seccount)

loc_800011D9:                           # CODE XREF: ATA_Transfer_Subfunc+52↑j
                movhu   (g_ATATransferLen), D0
                mov     ATA_DA58, A2
                asr     8, D0
                movbu   D0, (ATA_taskfile_bytecount_hi)
                movhu   (g_ATATransferLen), D0
                movbu   D0, (ATA_taskfile_bytecount_lo)
                mov     0x50, D0
                movbu   D0, (A2)
                clr     D0
                movbu   D0, (ATA_DA48)
                movbu   (ATA_taskfile_bytecount_hi), D0
                movbu   D0, (ATA_DA44)
                movbu   (ATA_taskfile_bytecount_lo), D0
                movbu   D0, (ATA_DA40)
                call    DealWithATATaskfileError, [], 0
                movbu   (ATA_taskfile_error), D0
                btst    1, D0
                bne     loc_80001216
                mov     8, D0
                movbu   D0, (A2)

loc_80001216:                           # CODE XREF: ATA_Transfer_Subfunc+90↑j
                mov     2, D0
                movbu   D0, (ATA_DA4C)
                ret     [A2], 8
# End of function ATA_Transfer_Subfunc


# =============== S U B R O U T I N E =======================================


DealWithATATaskfileError:               # CODE XREF: ATA_Transfer_Subfunc+85↑p
                movbu   (ATA_taskfile_error), D0
                btst    1, D0
                bne     loc_80001234
                movbu   (ATA_DA04), D0
                mov     ATA_DA04, A0
                and     0xBF, D0
                movbu   D0, (A0)
                jmp     loc_800012F9
# ---------------------------------------------------------------------------

loc_80001234:                           # CODE XREF: DealWithATATaskfileError+6↑j
                movbu   (ATA_taskfile_error), D0
                and     1, D0
                cmp     1, D0
                bne     loc_800012AD
                movbu   (byte_in_20), D0
                cmp     2, D0
                bne     loc_800012AD
                movbu   (ATA_DA04), D0
                mov     ATA_DA04, A0
                or      0x40, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                and     0xDF, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                or      0x10, D0
                movbu   D0, (A0)
                movbu   (byte_in_21), D0
                cmp     0, D0
                beq     loc_8000127F
                cmp     1, D0
                beq     loc_80001287
                cmp     2, D0
                beq     loc_8000128F
                cmp     3, D0
                beq     loc_80001297
                cmp     4, D0
                beq     loc_8000129F
                bra     loc_800012A5
# ---------------------------------------------------------------------------

loc_8000127F:                           # CODE XREF: DealWithATATaskfileError+4D↑j
                mov     0xC0, D0
                movbu   D0, (ATA_DAAC)
                bra     loc_800012A5
# ---------------------------------------------------------------------------

loc_80001287:                           # CODE XREF: DealWithATATaskfileError+51↑j
                mov     0xC1, D0
                movbu   D0, (ATA_DAAC)
                bra     loc_800012A5
# ---------------------------------------------------------------------------

loc_8000128F:                           # CODE XREF: DealWithATATaskfileError+55↑j
                mov     0xC2, D0
                movbu   D0, (ATA_DAAC)
                bra     loc_800012A5
# ---------------------------------------------------------------------------

loc_80001297:                           # CODE XREF: DealWithATATaskfileError+59↑j
                mov     0xC3, D0
                movbu   D0, (ATA_DAAC)
                bra     loc_800012A5
# ---------------------------------------------------------------------------

loc_8000129F:                           # CODE XREF: DealWithATATaskfileError+5D↑j
                mov     0xC4, D0
                movbu   D0, (ATA_DAAC)

loc_800012A5:                           # CODE XREF: DealWithATATaskfileError+5F↑j
                                        # DealWithATATaskfileError+67↑j ...
                mov     0x80, D0
                movbu   D0, (ATA_DA4C)
                bra     loc_800012F9
# ---------------------------------------------------------------------------

loc_800012AD:                           # CODE XREF: DealWithATATaskfileError+1E↑j
                                        # DealWithATATaskfileError+28↑j
                movbu   (ATA_taskfile_error), D0
                and     1, D0
                cmp     1, D0
                bne     loc_800012DE
                movbu   (byte_in_20), D0
                cmp     1, D0
                bne     loc_800012DE
                movbu   (ATA_DA04), D0
                mov     0xDA04, A0
                or      0x40, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                and     0xDF, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                and     0xEF, D0
                movbu   D0, (A0)
                bra     loc_800012F9
# ---------------------------------------------------------------------------

loc_800012DE:                           # CODE XREF: DealWithATATaskfileError+97↑j
                                        # DealWithATATaskfileError+A1↑j
                movbu   (ATA_DA04), D0
                mov     0xDA04, A0
                or      0x40, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                or      0x20, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                and     0xEF, D0
                movbu   D0, (A0)

loc_800012F9:                           # CODE XREF: DealWithATATaskfileError+13↑j
                                        # DealWithATATaskfileError+8D↑j ...
                movbu   (g_ATATransferDir), D0
                cmp     2, D0
                bne     loc_80001315
                movbu   (ATA_DA04), D0
                or      2, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                and     0xFB, D0
                movbu   D0, (A0)
                bra     locret_80001325
# ---------------------------------------------------------------------------

loc_80001315:                           # CODE XREF: DealWithATATaskfileError+E3↑j
                movbu   (ATA_DA04), D0
                or      2, D0
                movbu   D0, (A0)
                movbu   (ATA_DA04), D0
                or      4, D0
                movbu   D0, (A0)

locret_80001325:                        # CODE XREF: DealWithATATaskfileError+F5↑j
                rets
# End of function DealWithATATaskfileError


# =============== S U B R O U T I N E =======================================


TransferDeviceToHost:                   # CODE XREF: ATA_Transfer_Subfunc+3F↑p
                mov     SP, A0
                inc4    A0
                mov     A0, A1
                inc     A1
                bclr_s  4, byte_D95C
                clr     D2
                movbu   D2, (4,SP)
                movbu   D2, (A1)
                inc     A1
                movbu   D2, (A1)
                inc     A1
                movbu   D2, (A1)
                mov     D0, (0x10,SP)
                movbu   (0x10,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x11,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x12,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x13,SP), D0
                inc     A1
                movbu   D0, (A1)
                mov     D1, (0x10,SP)
                movbu   (0x10,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x11,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x12,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x13,SP), D0
                mov     0xC, D1
                movbu   D0, (1,A1)
                mov     4, D0
                call    TransferData, [D2], 4
                ret     [D2], 0x18
# End of function TransferDeviceToHost


# =============== S U B R O U T I N E =======================================


TransferHostToDevice:                   # CODE XREF: ATA_Transfer_Subfunc+2C↑p
                mov     SP, A0
                inc4    A0
                mov     A0, A1
                inc     A1
                bclr_s  4, byte_D95C
                clr     D2
                movbu   D2, (4,SP)
                movbu   D2, (A1)
                inc     A1
                movbu   D2, (A1)
                inc     A1
                movbu   D2, (A1)
                mov     D0, (0x10,SP)
                movbu   (0x10,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x11,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x12,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x13,SP), D0
                inc     A1
                movbu   D0, (A1)
                mov     D1, (0x10,SP)
                movbu   (0x10,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x11,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x12,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x13,SP), D0
                mov     0xC, D1
                movbu   D0, (1,A1)
                mov     5, D0
                call    TransferData, [D2], 4
                ret     [D2], 0x18
# End of function TransferHostToDevice


# =============== S U B R O U T I N E =======================================


TransferData:                           # CODE XREF: TransferDeviceToHost+4D↑p
                                        # TransferHostToDevice+4D↑p
                mov     0xDF14, A1
                setlb
                movbu   (byte_D904), D2
                and     0x30, D2
                cmp     0x20, D2
                lne
                setlb
                movbu   (byte_D905), D2
                btst    0x80, D2
                lne
                bra     loc_800013EE
# ---------------------------------------------------------------------------

loc_800013E8:                           # CODE XREF: TransferData+24↓j
                movbu   (A0), D2
                inc     A0
                movbu   D2, (A1)
                inc     A1

loc_800013EE:                           # CODE XREF: TransferData+15↑j
                mov     D1, D2
                extbu   D1
                .byte 0x29, 0xFF # add     0xFF, D1
                extbu   D2
                cmp     0, D2
                bne     loc_800013E8
                mov     4, D1
                movbu   D1, (byte_D954)
                movbu   D0, (byte_DF29)
                retf    [D2], 4
# End of function TransferData


# =============== S U B R O U T I N E =======================================


IOConf1:                                # CODE XREF: Cmd55+9↑p
                                        # SCSI_5A_ModeSense+8A↑p ...
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
# End of function IOConf1


# =============== S U B R O U T I N E =======================================


IOConf2:                                # CODE XREF: IOConf1+11↑p TransferSomeBytes+4D↓p
                mov     0xDF00, A1
                setlb
                movbu   (byte_D904), D2
                btst    0x80, D2
                lne
                bra     loc_8000142E
# ---------------------------------------------------------------------------

loc_80001428:                           # CODE XREF: IOConf2+1A↓j
                movbu   (A0), D2
                inc     A0
                movbu   D2, (A1)
                inc     A1

loc_8000142E:                           # CODE XREF: IOConf2+B↑j
                mov     D1, D2
                extbu   D1
                .byte 0x29, 0xFF # add     0xFF, D1
                extbu   D2
                cmp     0, D2
                bne     loc_80001428
                movbu   D0, (byte_DF28)
                retf    [D2], 4
# End of function IOConf2


# =============== S U B R O U T I N E =======================================


TransferSomeBytes:                                # CODE XREF: Cmd55+FC↑p
                mov     SP, A0
                inc4    A0
                mov     A0, A1
                inc     A1
                bclr_s  0x60, byte_D95C
                clr     D2
                movbu   D2, (4,SP)
                movbu   D2, (A1)
                inc     A1
                movbu   D2, (A1)
                inc     A1
                movbu   D2, (A1)
                mov     D0, (0x10,SP)
                movbu   (0x10,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x11,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x12,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x13,SP), D0
                inc     A1
                movbu   D0, (A1)
                mov     D1, (0x10,SP)
                movbu   (0x10,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x11,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x12,SP), D0
                inc     A1
                movbu   D0, (A1)
                movbu   (0x13,SP), D0
                mov     0xC, D1
                movbu   D0, (1,A1)
                mov     0x7A, D0
                call    IOConf2, [D2], 4

loc_8000148F:                           # CODE XREF: TransferSomeBytes+59↓j
                call    DoTransferSomeBytes, [], 0
                extb    D0
                cmp     D2, D0
                beq     loc_8000148F
                movbu   (g_MirrorDF34), D0
                movbu   D0, (0x10,SP)
                movbu   (g_MirrorDF35), D0
                movbu   D0, (0x11,SP)
                movbu   (g_MirrorDF36), D0
                movbu   D0, (0x12,SP)
                movbu   (g_MirrorDF37), D0
                movbu   D0, (0x13,SP)
                mov     (0x10,SP), D0
                ret     [D2], 0x18
# End of function TransferSomeBytes


# =============== S U B R O U T I N E =======================================


DoTransferSomeBytes:                                # CODE XREF: TransferSomeBytes:loc_8000148F↑p
                movbu   (byte_D954), D0
                and     0x60, D0
                cmp     0x60, D0
                beq     loc_800014CE
                clr     D0
                bra     locret_80001525
# ---------------------------------------------------------------------------

loc_800014CE:                           # CODE XREF: DoTransferSomeBytes+8↑j
                mov     0x60, D0
                movbu   D0, (byte_D954)
                setlb
                movbu   (byte_D904), D0
                btst    0x40, D0
                lne
                movbu   (byte_DF30), D0
                movbu   D0, (g_MirrorDF30)
                movbu   (byte_DF31), D0
                movbu   D0, (g_MirrorDF31)
                movbu   (byte_DF32), D0
                movbu   D0, (g_MirrorDF32)
                movbu   (byte_DF33), D0
                movbu   D0, (g_MirrorDF33)
                movbu   (byte_DF34), D0
                movbu   D0, (g_MirrorDF34)
                movbu   (byte_DF35), D0
                movbu   D0, (g_MirrorDF35)
                movbu   (byte_DF36), D0
                movbu   D0, (g_MirrorDF36)
                movbu   (byte_DF37), D0
                movbu   D0, (g_MirrorDF37)
                mov     1, D0

locret_80001525:                        # CODE XREF: DoTransferSomeBytes+B↑j
                rets
# End of function DoTransferSomeBytes


# =============== S U B R O U T I N E =======================================


SetATABytecount:                        # CODE XREF: Cmd55+BD↑p CmdNot55+CF↑p
                mov     D0, A2
                mov     FLASH_BASE, A1
                add     D1, A1
                bra     loc_80001547
# ---------------------------------------------------------------------------

loc_80001533:                           # CODE XREF: SetATABytecount+22↓j
                movbu   (A1), D0
                movbu   D0, (A2)
                movbu   (A1), D0
                inc     A1
                movbu   D0, (ATA_taskfile_bytecount_lo)
                movbu   (A2), D0
                inc     A2
                movbu   D0, (ATA_taskfile_bytecount_hi)
                mov     1, D0
                sub     D0, A0

loc_80001547:                           # CODE XREF: SetATABytecount+A↑j
                cmp     0, A0
                bne     loc_80001533
                retf    [A2], 4
# End of function SetATABytecount


# =============== S U B R O U T I N E =======================================


EraseForSomeVendorsAndProgram:          # CODE XREF: CmdNot55+345↑p
                mov     A0, A3
                mov     D1, (4,SP)
                movbu   (g_FlashVendorID), D1
                mov     A1, A2
                mov     D0, D3
                cmp     0xC2, D1
                beq     loc_80001563
                cmp     0x37, D1
                bne     loc_80001575

loc_80001563:                           # CODE XREF: EraseForSomeVendorsAndProgram+F↑j
                extbu   D0
                mov     (4,SP), D1
                mov     A1, D2
                extbu   D2
                mov     D2, A0
                call    EraseSector, [D2,D3,A2,A3], 0x18 # D1 = Sector
                extbu   D0
                cmp     0, D0
                beq     locret_80001581

loc_80001575:                           # CODE XREF: EraseForSomeVendorsAndProgram+13↑j
                extbu   D3
                mov     (4,SP), D1
                mov     A2, A1
                mov     A3, A0
                mov     D3, D0
                call    ProgramFlash, [D2,D3,A2,A3], 0x18
                extbu   D0

locret_80001581:                        # CODE XREF: EraseForSomeVendorsAndProgram+25↑j
                ret     [D2,D3,A2,A3], 0x18
# End of function EraseForSomeVendorsAndProgram


# =============== S U B R O U T I N E =======================================


EraseSectorAndProgram:                  # CODE XREF: SCSI_3B_WriteBuffer+1E0↑p
                mov     A0, A3
                mov     A1, A2
                mov     D1, (4,SP)
                mov     D0, D3
                extbu   D3
                mov     D3, D0
                mov     A2, D2
                extbu   D2
                mov     D2, A0
                call    EraseSector, [D2,D3,A2,A3], 0x18 # D1 = Sector
                mov     (4,SP), D1
                mov     A2, A1
                mov     A3, A0
                mov     D3, D0
                call    ProgramFlash, [D2,D3,A2,A3], 0x18
                extbu   D0
                ret     [D2,D3,A2,A3], 0x18
# End of function EraseSectorAndProgram


# =============== S U B R O U T I N E =======================================


EraseChipOrSector:                      # CODE XREF: SCSI_3B_WriteBuffer+69↑p
                movbu   (g_EntrypointCmdMode), D0
                cmp     0, D0
                bne     loc_800015DD
                movbu   (g_FlashTypeCol4), D0
                movbu   (g_FlashTypeID_Always2), D1
                call    EraseChip, [D2,D3,A2,A3], 0x18
                extbu   D0
                cmp     0, D0
                beq     loc_800015C6
                ret     [D2,D3,A2,A3], 0x14
# ---------------------------------------------------------------------------

loc_800015C6:                           # CODE XREF: EraseChipOrSector+1E↑j
                mov     0xD4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

loc_800015DB:                           # CODE XREF: EraseChipOrSector:loc_800015DB↓j
                bra     loc_800015DB
# ---------------------------------------------------------------------------

loc_800015DD:                           # CODE XREF: EraseChipOrSector+8↑j
                movbu   (g_FlashVendorID), D0
                mov     ROMMap_Atmel, A0
                mov     A0, (g_pROMMap)
                cmp     0x1F, D0
                beq     loc_80001627
                mov     ROMMap_Macronix, A0
                mov     A0, (g_pROMMap)
                cmp     0xC2, D0
                beq     loc_80001627
                cmp     0x37, D0
                beq     loc_80001627
                mov     ROMMap_SST, A0
                mov     A0, (g_pROMMap)
                cmp     0xBF, D0
                beq     loc_80001627
                mov     ROMMap_PMC, A0
                mov     A0, (g_pROMMap)

loc_80001627:                           # CODE XREF: EraseChipOrSector+4E↑j
                                        # EraseChipOrSector+60↑j ...
                mov     0xBF, A3
                cmp     A3, D0
                beq     loc_80001633
                cmp     0x9D, D0
                bne     loc_8000163A

loc_80001633:                           # CODE XREF: EraseChipOrSector+88↑j
                mov     0x6000, D3
                mov     4, D2
                bra     loc_800016A9
# ---------------------------------------------------------------------------

loc_8000163A:                           # CODE XREF: EraseChipOrSector+8E↑j
                add     0xC, A0
                mov     A0, (g_pROMMap)
                mov     (A0), D3
                movbu   (8,A0), D2
                bra     loc_800016A9
# ---------------------------------------------------------------------------

loc_80001648:                           # CODE XREF: EraseChipOrSector+108↓j
                movbu   (g_FlashTypeCol3), D0
                mov     D3, D1
                extbu   D2
                mov     D2, A0
                call    EraseSector, [D2,D3,A2,A3], 0x18 # D1 = Sector
                extbu   D0
                cmp     0, D0
                bne     loc_80001673
                mov     0xD4, D0
                movbu   D0, (ATA_taskfile_error)
                mov     0x51, D0  # ERR|DRDY|DSC
                movbu   D0, (ATA_taskfile_status)
                mov     3, D0
                movbu   D0, (ATA_taskfile_seccount)
                mov     1, D0
                movbu   D0, (ATA_InterruptFlag)

loc_80001671:                           # CODE XREF: EraseChipOrSector:loc_80001671↓j
                bra     loc_80001671
# ---------------------------------------------------------------------------

loc_80001673:                           # CODE XREF: EraseChipOrSector+B7↑j
                movbu   (g_FlashVendorID), D0
                cmp     A3, D0
                beq     loc_80001683
                cmp     0x9D, D0
                bne     loc_80001695

loc_80001683:                           # CODE XREF: EraseChipOrSector+D8↑j
                add     0x1000, D3
                mov     4, D2
                cmp     0x40000, D3
                bne     loc_800016A9
                .byte 0x8F, 0xFF # mov     0xFF, D3
                bra     loc_800016A9
# ---------------------------------------------------------------------------

loc_80001695:                           # CODE XREF: EraseChipOrSector+DE↑j
                mov     (g_pROMMap), D0
                mov     0xC, A0
                add     D0, A0
                mov     A0, (g_pROMMap)
                mov     (A0), D3
                movbu   (8,A0), D2

loc_800016A9:                           # CODE XREF: EraseChipOrSector+95↑j
                                        # EraseChipOrSector+A3↑j ...
                .byte 0xAF, 0xFF # cmp     0xFF, D3
                bne     loc_80001648
                ret     [D2,D3,A2,A3], 0x14
# End of function EraseChipOrSector






# ---------------------------------------------------------------------------
# Atmel
g_FlashTable_0: .byte 0x1F, 7, 2, 4, 1, 1        # DATA XREF: CheckFlashType:loc_80000BEA↑o
                .byte 0x1F, 8, 2, 3, 1, 1
                .byte 0xFF, 0, 0, 0, 0, 0

                .byte 0, 0  # padding
ROMMap_Atmel:      .long 0                   # DATA XREF: SCSI_3B_WriteBuffer+74↑o
                                        # CmdNot55+117↑o ...
                .long 0x3FFF
                .long 0x10
                .long 0x6000
                .long 0x7FFF
                .long 8
                .long 0x8000
                .long 0xFFFF
                .long 0x20
                .long 0x10000
                .long 0x1FFFF
                .long 0x40
                .long 0x20000
                .long 0x2FFFF
                .long 0x40
                .long 0x30000
                .long 0x3FFFF
                .long 0x40
                .long 0xFFFFFFFF
                .long 0xFFFFFFFF
                .long 0xFF
ROMMap_Atmel_4000:.long 0x4000              # DATA XREF: Cmd55+6A↑o
                                        # SCSI_3B_WriteBuffer+187↑o ...
                .long 0x5FFF
                .long 8
# Macronix/AMIC
g_FlashTable_1: .byte 0xC2, 0x34, 2, 0, 0, 0       # DATA XREF: CheckFlashType:loc_80000CCB↑o
                .byte 0xC2, 0xB0, 2, 0, 0, 0
                .byte 0x37, 0xD,  2, 0, 0, 0
                .byte 0xFF, 0,    0, 0, 0, 0
ROMMap_Macronix:      .long 0                   # DATA XREF: SCSI_3B_WriteBuffer+84↑o
                                        # CmdNot55+127↑o ...
                .long 0x3FFF
                .long 0x10
                .long 0x6000
                .long 0x7FFF
                .long 8
                .long 0x8000
                .long 0xFFFF
                .long 0x20
                .long 0x10000
                .long 0x1FFFF
                .long 0x40
                .long 0x20000
                .long 0x2FFFF
                .long 0x40
                .long 0x30000
                .long 0x3FFFF
                .long 0x40
                .long 0xFFFFFFFF
                .long 0xFFFFFFFF
                .long 0xFF
ROMMap_Macronix_4000:.long 0x4000              # DATA XREF: Cmd55+7A↑o
                                        # SCSI_3B_WriteBuffer+197↑o ...
                .long 0x5FFF
                .long 8
# SST
g_FlashTable_2: .byte 0xBF, 0xB6, 2, 5, 1, 1    # DATA XREF: CheckFlashType:SetEntryList↑o
                .byte 0xBF, 0x86, 2, 5, 1, 1
                .byte 0xFF, 0,    0, 0, 0, 0

                .byte 0, 0  # padding
ROMMap_SST: .long 0                   # DATA XREF: SCSI_3B_WriteBuffer+9A↑o
                                        # CmdNot55+13D↑o ...
                .long 0x3FFF
                .long 0x10
                .long 0x6000
                .long 0x3FFFF
                .long 0xE8
                .long 0xFFFFFFFF
                .long 0xFFFFFFFF
                .long 0xFF
ROMMap_SST_4000:.long 0x4000              # DATA XREF: Cmd55+90↑o
                                        # SCSI_3B_WriteBuffer+1AD↑o ...
                .long 0x5FFF
                .long 8
# PMC
g_FlashTable_3: .byte 0x9D, 0x4D, 2, 0, 0, 0  # DATA XREF: CheckFlashType:loc_80000D25↑o
                .byte 0xFF, 0,    0, 0, 0, 0
ROMMap_PMC:.long 0                  # DATA XREF: SCSI_3B_WriteBuffer+AC↑o
                                        # CmdNot55+14F↑o ...
                .long 0x3FFF
                .long 0x10
                .long 0x6000
                .long 0x3FFFF
                .long 0xE8
                .long 0xFFFFFFFF
                .long 0xFFFFFFFF
                .long 0xFF
ROMMap_PMC_4000:.long 0x4000         # DATA XREF: Cmd55+A2↑o
                                        # SCSI_3B_WriteBuffer+1BF↑o ...
                .long 0x5FFF
                .long 8
# ----------

g_ATATransferPtr:  .long 0xFFFFFFFF

g_dwCurFlashOffset:.long 0xFFFFFFFF
g_DelayStore:   .long 0xFFFFFFFF
g_ATATransferLen: .word 0xFFFF
g_ATATransferDir: .byte 0xFF

g_FlashTypeCol3:  .byte 0xFF                # DATA XREF: Cmd55+1E↑o CmdNot55+1E↑o ...
g_FlashTypeID_Always2:  .byte 0xFF          # DATA XREF: Cmd55+24↑o
                                        # Cmd55:loc_800000AC↑r ...
g_FlashTypeCol4:  .byte 0xFF                # DATA XREF: Cmd55+2A↑o CmdNot55+2A↑o ...
# Whether to use 2AA or 2AAA etc.
g_FlashAddressingType:  .byte 0xFF          # DATA XREF: Cmd55+30↑o
                                        # SCSI_3B_WriteBuffer+125↑r ...
g_FlashVendorID:  .byte 0xFF                # DATA XREF: Cmd55+E↑o Cmd55+64↑r ...
g_FlashDeviceID:.byte 0xFF                # DATA XREF: Cmd55+16↑o CmdNot55+16↑o ...

g_CDB2_From_SCSI_3B:.byte 0xFF
g_MirrorDF30:   .byte 0xFF     # never read
g_MirrorDF31:   .byte 0xFF     # never read
g_MirrorDF32:   .byte 0xFF     # never read
g_MirrorDF33:   .byte 0xFF     # never read
g_MirrorDF34:   .byte 0xFF
g_MirrorDF35:   .byte 0xFF
g_MirrorDF36:   .byte 0xFF
g_MirrorDF37:   .byte 0xFF
g_EntrypointCmdMode:   .byte 0xFF
g_bAllowErase:  .byte 0xFF
g_bWantErase:   .byte 0xFF
g_bReallyWantErase:.byte 0xFF
                .byte 0xFF, 0xFF  # padding
# void *g_pROMMap
g_pROMMap:       .long 0xFFFFFFFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xFF
                .byte 0xB2
                .byte    3
