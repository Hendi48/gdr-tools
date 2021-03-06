/*
 * Will program single sectors of the
 * firmware flash chip within the
 * Hitachi-LG Xbox 360 DVD drive.
 *
 * example: Imagine you've modified the sector at 0x3F000 in firmware.bin
 *          and you want to flash this new sector to drive /dev/hdc which
 *          has a flash chip organised in 4KB sectors (0x1000 bytes).
 *
 * $ flashsec47 /dev/hdc firmware.bin 9003F000 1000
 *
 * author: Kevin East (SeventhSon)
 * email: kev@kev.nu
 * web: http://www.kev.nu/360/
 * date: 16th May 2006
 * platform: linux
 *
 */

#include <stdio.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

// Don't change the size of any of these code blocks, you'll mess up the hard coded offsets

// Control code
/*
2 movm [D2,D3,A2,A3,D0,D1,A0,A1,MDR,LIR,LAR], (SP) // CF F8
4 and 0xFDFF, PSW // FA FC FF FD
6 mov dest_address, D0 // FC CC ?? ?? ?? ??
3 mov D0, (934) // 01 34 09
6 calls erase_code // FC FF 36 00 00 00
3 movbu (92E), D0 // 34 2E 09
2 cmp 0, D0 // A0 00
2 bne exit // C9 23 (disp.)
6 mov dest_address, D0 // FC CC ?? ?? ?? ??
3 mov D0, (934) // 01 34 09
6 mov src_address, D0 // FC CC 81 08 00 80
3 mov D0, (938) // 01 38 09
6 mov sector_size, D0 // FC CC ?? ?? ?? ??
3 mov D0, (92C) // 01 2C 09
6 calls program_code // FC FF 7E 04 00 00
exit:
4 or 0x200, PSW // FA FD 00 02
2 movm (SP), [D2,D3,A2,A3,D0,D1,A0,A1,MDR,LIR,LAR] // CE F8
2 rets // F0 FC
*/
unsigned char control_code[] = {
0xCF,0xF8,0xFA,0xFC,0xFF,0xFD,0xFC,0xCC,0x00,0x00,0x00,0x00,0x01,0x34,0x09,0xFC,
0xFF,0x36,0x00,0x00,0x00,0x34,0x2E,0x09,0xA0,0x00,0xC9,0x23,0xFC,0xCC,0x00,0x00,
0x00,0x00,0x01,0x34,0x09,0xFC,0xCC,0x81,0x08,0x00,0x80,0x01,0x38,0x09,0xFC,0xCC,
0x00,0x00,0x00,0x00,0x01,0x2C,0x09,0xFC,0xFF,0x7E,0x04,0x00,0x00,0xFA,0xFD,0x00,
0x02,0xCE,0xF8,0xF0,0xFC
};

// Flash sector erase code (from Hitchi-LG 3120L firmware revision 0047)
// This code was modified slightly to prevent it from corrupting the
// sector source data in RAM (placed after the code in our buffer) and
// to prevent erasing two sectors at once on 4K sector flash devices.
unsigned char erase_code[] = {
0xCF,0x08,0xCF,0x20,0xF8,0xFE,0xFC,0xFC,0xFF,0x29,0x04,0x00,0x00,0xF8,0xFE,0x04,
0x34,0xA8,0xD9,0x85,0x01,0xF2,0x14,0x02,0xA8,0xD9,0xF8,0xFE,0xFC,0xFC,0xFF,0x76,
0x00,0x00,0x00,0xF8,0xFE,0x04,0xFC,0xA8,0x20,0x30,0x00,0x80,0x2D,0xFF,0x00,0xA1,
0xC8,0x39,0x80,0x05,0xFC,0x81,0x00,0x30,0x00,0x80,0xF8,0xFE,0xFC,0xFC,0xFF,0x80,
0x02,0x00,0x00,0xF8,0xFE,0x04,0xF8,0xFE,0xFC,0xFC,0xFF,0xA2,0x03,0x00,0x00,0xF8,
0xFE,0x04,0x81,0xC8,0x1E,0xFC,0xA4,0x00,0x30,0x00,0x80,0xFC,0xC4,0x01,0x00,0x00,
0x00,0xFC,0x81,0x00,0x30,0x00,0x80,0xC9,0xD3,0x2C,0xFF,0x00,0x02,0x2E,0x09,0xCA,
0x07,0x80,0x00,0x02,0x2E,0x09,0x34,0xA8,0xD9,0x2D,0xFE,0x00,0xF2,0x04,0x02,0xA8,
0xD9,0xF8,0xFE,0xFC,0xFC,0xFF,0xCC,0x03,0x00,0x00,0xF8,0xFE,0x04,0xCE,0x20,0xCE,
0x08,0xF0,0xFC,0xFC,0xDC,0x00,0x00,0x00,0x90,0xFA,0xD0,0x55,0x55,0x2C,0xAA,0x00,
0xF0,0x50,0xFC,0xDD,0x00,0x00,0x00,0x90,0xFA,0xD1,0xAA,0x2A,0x80,0x55,0xF0,0x51,
0x2C,0x90,0x00,0xF0,0x50,0xCB,0xCB,0xFC,0xA8,0x00,0x00,0x00,0x90,0xFC,0x82,0x20,
0x30,0x00,0x80,0xFC,0xA8,0x00,0x10,0x00,0x90,0xFC,0x82,0x24,0x30,0x00,0x80,0xFC,
0xA8,0x00,0x20,0x00,0x90,0xFC,0x82,0x28,0x30,0x00,0x80,0xFC,0xA8,0x01,0x00,0x00,
0x90,0xFC,0x82,0x04,0x30,0x00,0x80,0x2C,0xAA,0x00,0xF0,0x50,0x80,0x55,0xF0,0x51,
0x2C,0xF0,0x00,0xF0,0x50,0xCB,0xCB,0xF8,0xFE,0xFC,0xFC,0xFF,0x16,0x03,0x00,0x00,
0xF8,0xFE,0x04,0x85,0x1F,0xFC,0xA8,0x20,0x30,0x00,0x80,0xA4,0xC9,0x20,0xFC,0xA8,
0x24,0x30,0x00,0x80,0xA4,0xC9,0x17,0xFC,0xA8,0x28,0x30,0x00,0x80,0xA4,0xC9,0x0E,
0x2C,0x00,0x20,0xFC,0x81,0x2C,0x30,0x00,0x80,0xCC,0x92,0x01,0xFC,0xDC,0x00,0x00,
0x00,0x90,0xFA,0xD0,0x55,0x05,0x2C,0xAA,0x00,0xF0,0x50,0xFC,0xDD,0x00,0x00,0x00,
0x90,0xFA,0xD1,0xAA,0x02,0x80,0x55,0xF0,0x51,0x2C,0x90,0x00,0xF0,0x50,0xCB,0xCB,
0xFC,0xA8,0x00,0x00,0x00,0x90,0xFC,0x82,0x20,0x30,0x00,0x80,0xFC,0xA8,0x00,0x10,
0x00,0x90,0xFC,0x82,0x24,0x30,0x00,0x80,0xFC,0xA8,0x00,0x20,0x00,0x90,0xFC,0x82,
0x28,0x30,0x00,0x80,0xFC,0xA8,0x01,0x00,0x00,0x90,0xFC,0x82,0x04,0x30,0x00,0x80,
0x2C,0xAA,0x00,0xF0,0x50,0x80,0x55,0xF0,0x51,0x2C,0xF0,0x00,0xF0,0x50,0xCB,0xCB,
0xF8,0xFE,0xFC,0xFC,0xFF,0x7D,0x02,0x00,0x00,0xF8,0xFE,0x04,0x2D,0xC2,0x00,0xFC,
0xA8,0x20,0x30,0x00,0x80,0xA4,0xC9,0x20,0xFC,0xA8,0x24,0x30,0x00,0x80,0xA4,0xC9,
0x17,0xFC,0xA8,0x28,0x30,0x00,0x80,0xA4,0xC9,0x0E,0x2C,0x00,0x20,0xFC,0x81,0x2C,
0x30,0x00,0x80,0xCC,0xF8,0x00,0x2D,0x9D,0x00,0xFC,0xA8,0x20,0x30,0x00,0x80,0xA4,
0xC9,0x20,0xFC,0xA8,0x24,0x30,0x00,0x80,0xA4,0xC9,0x17,0xFC,0xA8,0x28,0x30,0x00,
0x80,0xA4,0xC9,0x0E,0x2C,0x00,0x10,0xFC,0x81,0x2C,0x30,0x00,0x80,0xCC,0xCE,0x00,
0x85,0x37,0xFC,0xA8,0x20,0x30,0x00,0x80,0xA4,0xC9,0x20,0xFC,0xA8,0x24,0x30,0x00,
0x80,0xA4,0xC9,0x17,0xFC,0xA8,0x28,0x30,0x00,0x80,0xA4,0xC9,0x0E,0x2C,0x00,0x20,
0xFC,0x81,0x2C,0x30,0x00,0x80,0xCC,0xA5,0x00,0xFC,0xDC,0x00,0x00,0x00,0x90,0xFA,
0xD0,0x55,0x55,0x2C,0xAA,0x00,0xF0,0x50,0xFC,0xDD,0x00,0x00,0x00,0x90,0xFA,0xD1,
0xAA,0x2A,0x80,0x55,0xF0,0x51,0x2C,0x90,0x00,0xF0,0x50,0xCB,0xCB,0xFC,0xA8,0x00,
0x00,0x00,0x90,0xFC,0x82,0x20,0x30,0x00,0x80,0xFC,0xA8,0x00,0x00,0x00,0x90,0xFC,
0x82,0x24,0x30,0x00,0x80,0xFC,0xA8,0x00,0x00,0x00,0x90,0xFC,0x82,0x28,0x30,0x00,
0x80,0xFC,0xA8,0x01,0x00,0x00,0x90,0xFC,0x82,0x04,0x30,0x00,0x80,0x2C,0xAA,0x00,
0xF0,0x50,0x80,0x55,0xF0,0x51,0x2C,0xF0,0x00,0xF0,0x50,0xCB,0xCB,0xF8,0xFE,0xFC,
0xFC,0xFF,0x90,0x01,0x00,0x00,0xF8,0xFE,0x04,0x2D,0xBF,0x00,0xFC,0xA8,0x20,0x30,
0x00,0x80,0xA4,0xC9,0x1F,0xFC,0xA8,0x24,0x30,0x00,0x80,0xA4,0xC9,0x16,0xFC,0xA8,
0x28,0x30,0x00,0x80,0xA4,0xC9,0x0D,0x2C,0x00,0x10,0xFC,0x81,0x2C,0x30,0x00,0x80,
0xCA,0x0B,0x2C,0xFF,0x00,0xFC,0x82,0x20,0x30,0x00,0x80,0xF0,0xFC,0xFC,0xA8,0x20,
0x30,0x00,0x80,0x85,0x1F,0xA4,0xC8,0x19,0x2D,0xC2,0x00,0xA4,0xC8,0x55,0x2D,0x9D,
0x00,0xA4,0xC9,0x05,0xCC,0x8F,0x00,0x85,0x37,0xA4,0xC8,0x47,0xCC,0x9D,0x00,0xFC,
0xDC,0x00,0x00,0x00,0x90,0xFA,0xD0,0x55,0x55,0x2C,0xAA,0x00,0xF0,0x50,0xFC,0xDD,
0x00,0x00,0x00,0x90,0xFA,0xD1,0xAA,0x2A,0x80,0x55,0xF0,0x51,0x2C,0x80,0x00,0xF0,
0x50,0x2C,0xAA,0x00,0xF0,0x50,0x80,0x55,0xF0,0x51,0xFA,0xA0,0x34,0x09,0x80,0x30,
0xF0,0x50,0xF8,0xFE,0xFC,0xFC,0xFF,0xFB,0x00,0x00,0x00,0xF8,0xFE,0x04,0xCC,0xCB,
0x00,0xFC,0xDC,0x00,0x00,0x00,0x90,0xFA,0xD0,0x55,0x05,0x2C,0xAA,0x00,0xF0,0x50,
0xFC,0xDD,0x00,0x00,0x00,0x90,0xFA,0xD1,0xAA,0x02,0x80,0x55,0xF0,0x51,0x2C,0x80,
0x00,0xF0,0x50,0x2C,0xAA,0x00,0xF0,0x50,0x80,0x55,0xF0,0x51,0xFA,0xA0,0x34,0x09,
0x80,0x30,0xF0,0x50,0xF8,0xFE,0xFC,0xFC,0xFF,0xB9,0x00,0x00,0x00,0xF8,0xFE,0x04,
0xCC,0x89,0x00,0xFC,0xDC,0x00,0x00,0x00,0x90,0xFA,0xD0,0x55,0x05,0xFC,0xDD,0x00,
0x00,0x00,0x90,0xFA,0xD1,0xAA,0x02,0xCA,0x16,0xFC,0xDC,0x00,0x00,0x00,0x90,0xFA,
0xD0,0x55,0x55,0xFC,0xDD,0x00,0x00,0x00,0x90,0xFA,0xD1,0xAA,0x2A,0x2C,0xAA,0x00,
0xF0,0x50,0x80,0x55,0xF0,0x51,0x2C,0x80,0x00,0xF0,0x50,0x2C,0xAA,0x00,0xF0,0x50,
0x80,0x55,0xF0,0x51,0xFA,0xA2,0x34,0x09,0x80,0x30,0xF0,0x52,0xF8,0xFE,0xFC,0xFC,
0xFF,0x61,0x00,0x00,0x00,0xF8,0xFE,0x04,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,
0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,
0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,
0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCB,0xCA,0x02,0xF0,0xFC,0xFC,0xA5,0x2C,0x30,0x00,
0x80,0xFA,0xA0,0x34,0x09,0xF0,0x40,0xFA,0xC8,0xFF,0x00,0xC9,0x10,0x41,0xFC,0xC5,
0x01,0x00,0x00,0x00,0xC9,0xF1,0x80,0x00,0xDF,0x00,0x00,0x2C,0xFF,0x00,0xF0,0xFC,
0xFC,0xA8,0x00,0x00,0x00,0x90,0xCB,0xCB,0xFC,0xA9,0x00,0x00,0x00,0x90,0xCB,0xCB,
0xA1,0xC9,0xEF,0xFC,0xA9,0x00,0x00,0x00,0x90,0xCB,0xCB,0xA1,0xC9,0xE4,0xF0,0xFC,
0x34,0x04,0xD9,0xF8,0xE0,0x80,0xC9,0xFA,0xCB,0xCB,0x80,0x01,0x02,0x00,0xDF,0xCB,
0xCB,0x80,0x00,0x02,0x01,0xDF,0xCB,0xCB,0x2C,0x9A,0x00,0x02,0x28,0xDF,0xF0,0xFC,
0x34,0x04,0xD9,0xF8,0xE0,0x80,0xC9,0xFA,0xCB,0xCB,0x80,0x00,0x02,0x00,0xDF,0xCB,
0xCB,0x80,0x00,0x02,0x01,0xDF,0xCB,0xCB,0x2C,0x9A,0x00,0x02,0x28,0xDF,0xF0,0xFC
};

// Flash byte-program code (from Hitchi-LG 3120L firmware revision 0047)
// This code was modified slightly to prevent it from corrupting the
// sector source data in RAM (placed after the code in our buffer).
unsigned char program_code[] = {
0xCF,0x08,0xF8,0xFE,0xFC,0xFC,0xFF,0x87,0x03,0x00,0x00,0xF8,0xFE,0x04,0x34,0xA8,
0xD9,0x85,0x01,0xF2,0x14,0x02,0xA8,0xD9,0xF8,0xFE,0xFC,0xFC,0xFF,0xA3,0x00,0x00,
0x00,0xF8,0xFE,0x04,0xFC,0xA8,0x20,0x30,0x00,0x80,0x2D,0xFF,0x00,0xA1,0xC8,0x42,
0x38,0x2C,0x09,0xFC,0x81,0x2C,0x30,0x00,0x80,0x80,0x0A,0xFC,0x81,0x00,0x30,0x00,
0x80,0xF8,0xFE,0xFC,0xFC,0xFF,0x79,0x02,0x00,0x00,0xF8,0xFE,0x04,0xF8,0xFE,0xFC,
0xFC,0xFF,0x03,0x03,0x00,0x00,0xF8,0xFE,0x04,0x81,0xC8,0x1E,0xFC,0xA4,0x00,0x30,
0x00,0x80,0xFC,0xC4,0x01,0x00,0x00,0x00,0xFC,0x81,0x00,0x30,0x00,0x80,0xC9,0xD3,
0x2C,0xFF,0x00,0x02,0x2E,0x09,0xCA,0x2D,0xFA,0xA0,0x34,0x09,0x41,0xFA,0x80,0x34,
0x09,0xFA,0xA0,0x38,0x09,0x41,0xFA,0x80,0x38,0x09,0xFC,0xA4,0x2C,0x30,0x00,0x80,
0xFC,0xC4,0x01,0x00,0x00,0x00,0xFC,0x81,0x2C,0x30,0x00,0x80,0xC9,0x9D,0x80,0x00,
0x02,0x2E,0x09,0x34,0xA8,0xD9,0x2D,0xFE,0x00,0xF2,0x04,0x02,0xA8,0xD9,0xF8,0xFE,
0xFC,0xFC,0xFF,0xFB,0x02,0x00,0x00,0xF8,0xFE,0x04,0xCE,0x08,0xF0,0xFC,0xFC,0xDC,
0x00,0x00,0x00,0x90,0xFA,0xD0,0x55,0x55,0x2C,0xAA,0x00,0xF0,0x50,0xFC,0xDD,0x00,
0x00,0x00,0x90,0xFA,0xD1,0xAA,0x2A,0x80,0x55,0xF0,0x51,0x2C,0x90,0x00,0xF0,0x50,
0xCB,0xCB,0xFC,0xA8,0x00,0x00,0x00,0x90,0xFC,0x82,0x20,0x30,0x00,0x80,0xFC,0xA8,
0x00,0x10,0x00,0x90,0xFC,0x82,0x24,0x30,0x00,0x80,0xFC,0xA8,0x00,0x20,0x00,0x90,
0xFC,0x82,0x28,0x30,0x00,0x80,0xFC,0xA8,0x01,0x00,0x00,0x90,0xFC,0x82,0x04,0x30,
0x00,0x80,0x2C,0xAA,0x00,0xF0,0x50,0x80,0x55,0xF0,0x51,0x2C,0xF0,0x00,0xF0,0x50,
0xCB,0xCB,0xF8,0xFE,0xFC,0xFC,0xFF,0x47,0x02,0x00,0x00,0xF8,0xFE,0x04,0x85,0x1F,
0xFC,0xA8,0x20,0x30,0x00,0x80,0xA4,0xC9,0x19,0xFC,0xA8,0x24,0x30,0x00,0x80,0xA4,
0xC9,0x10,0xFC,0xA8,0x28,0x30,0x00,0x80,0xA4,0xC9,0x07,0xC9,0x05,0xCC,0x6E,0x01,
0xFC,0xDC,0x00,0x00,0x00,0x90,0xFA,0xD0,0x55,0x05,0x2C,0xAA,0x00,0xF0,0x50,0xFC,
0xDD,0x00,0x00,0x00,0x90,0xFA,0xD1,0xAA,0x02,0x80,0x55,0xF0,0x51,0x2C,0x90,0x00,
0xF0,0x50,0xCB,0xCB,0xFC,0xA8,0x00,0x00,0x00,0x90,0xFC,0x82,0x20,0x30,0x00,0x80,
0xFC,0xA8,0x00,0x10,0x00,0x90,0xFC,0x82,0x24,0x30,0x00,0x80,0xFC,0xA8,0x00,0x20,
0x00,0x90,0xFC,0x82,0x28,0x30,0x00,0x80,0xFC,0xA8,0x01,0x00,0x00,0x90,0xFC,0x82,
0x04,0x30,0x00,0x80,0x2C,0xAA,0x00,0xF0,0x50,0x80,0x55,0xF0,0x51,0x2C,0xF0,0x00,
0xF0,0x50,0xCB,0xCB,0xF8,0xFE,0xFC,0xFC,0xFF,0xB5,0x01,0x00,0x00,0xF8,0xFE,0x04,
0x2D,0xC2,0x00,0xFC,0xA8,0x20,0x30,0x00,0x80,0xA4,0xC9,0x17,0xFC,0xA8,0x24,0x30,
0x00,0x80,0xA4,0xC9,0x0E,0xFC,0xA8,0x28,0x30,0x00,0x80,0xA4,0xC9,0x05,0xCC,0xDD,
0x00,0x2D,0x9D,0x00,0xFC,0xA8,0x20,0x30,0x00,0x80,0xA4,0xC9,0x17,0xFC,0xA8,0x24,
0x30,0x00,0x80,0xA4,0xC9,0x0E,0xFC,0xA8,0x28,0x30,0x00,0x80,0xA4,0xC9,0x05,0xCC,
0xBC,0x00,0x85,0x37,0xFC,0xA8,0x20,0x30,0x00,0x80,0xA4,0xC9,0x17,0xFC,0xA8,0x24,
0x30,0x00,0x80,0xA4,0xC9,0x0E,0xFC,0xA8,0x28,0x30,0x00,0x80,0xA4,0xC9,0x05,0xCC,
0x9C,0x00,0xFC,0xDC,0x00,0x00,0x00,0x90,0xFA,0xD0,0x55,0x55,0x2C,0xAA,0x00,0xF0,
0x50,0xFC,0xDD,0x00,0x00,0x00,0x90,0xFA,0xD1,0xAA,0x2A,0x80,0x55,0xF0,0x51,0x2C,
0x90,0x00,0xF0,0x50,0xCB,0xCB,0xFC,0xA8,0x00,0x00,0x00,0x90,0xFC,0x82,0x20,0x30,
0x00,0x80,0xFC,0xA8,0x00,0x00,0x00,0x90,0xFC,0x82,0x24,0x30,0x00,0x80,0xFC,0xA8,
0x00,0x00,0x00,0x90,0xFC,0x82,0x28,0x30,0x00,0x80,0xFC,0xA8,0x01,0x00,0x00,0x90,
0xFC,0x82,0x04,0x30,0x00,0x80,0x2C,0xAA,0x00,0xF0,0x50,0x80,0x55,0xF0,0x51,0x2C,
0xF0,0x00,0xF0,0x50,0xCB,0xCB,0xF8,0xFE,0xFC,0xFC,0xFF,0xE3,0x00,0x00,0x00,0xF8,
0xFE,0x04,0x2D,0xBF,0x00,0xFC,0xA8,0x20,0x30,0x00,0x80,0xA4,0xC9,0x16,0xFC,0xA8,
0x24,0x30,0x00,0x80,0xA4,0xC9,0x0D,0xFC,0xA8,0x28,0x30,0x00,0x80,0xA4,0xC9,0x04,
0xC8,0x0B,0x2C,0xFF,0x00,0xFC,0x82,0x20,0x30,0x00,0x80,0xF0,0xFC,0xFC,0xA8,0x20,
0x30,0x00,0x80,0x85,0x1F,0xA4,0xC8,0x15,0x2D,0xC2,0x00,0xA4,0xC8,0x4B,0x2D,0x9D,
0x00,0xA4,0xC8,0x45,0x85,0x37,0xA4,0xC8,0x40,0xCA,0x02,0xFC,0xDC,0x00,0x00,0x00,
0x90,0xFA,0xD0,0x55,0x55,0x2C,0xAA,0x00,0xF0,0x50,0xFC,0xDD,0x00,0x00,0x00,0x90,
0xFA,0xD1,0xAA,0x2A,0x80,0x55,0xF0,0x51,0x2C,0xA0,0x00,0xF0,0x50,0xFA,0xA1,0x34,
0x09,0xFA,0xA0,0x38,0x09,0xF0,0x40,0xF0,0x51,0xF8,0xFE,0xFC,0xFC,0xFF,0x60,0x00,
0x00,0x00,0xF8,0xFE,0x04,0xCA,0x3C,0xFC,0xDC,0x00,0x00,0x00,0x90,0xFA,0xD0,0x55,
0x05,0x2C,0xAA,0x00,0xF0,0x50,0xFC,0xDD,0x00,0x00,0x00,0x90,0xFA,0xD1,0xAA,0x02,
0x80,0x55,0xF0,0x51,0x2C,0xA0,0x00,0xF0,0x50,0xFA,0xA1,0x34,0x09,0xFA,0xA0,0x38,
0x09,0xF0,0x40,0xF0,0x51,0xF8,0xFE,0xFC,0xFC,0xFF,0x24,0x00,0x00,0x00,0xF8,0xFE,
0x04,0xF0,0xFC,0xFA,0xA1,0x34,0x09,0xFA,0xA0,0x38,0x09,0xF0,0x40,0xF0,0x45,0xA4,
0xC9,0x07,0x80,0x00,0xDF,0x00,0x00,0x2C,0xFF,0x00,0xF0,0xFC,0xFC,0xA8,0x00,0x00,
0x00,0x90,0xCB,0xCB,0xFC,0xA9,0x00,0x00,0x00,0x90,0xCB,0xCB,0xA1,0xC9,0xEF,0xFC,
0xA9,0x00,0x00,0x00,0x90,0xCB,0xCB,0xA1,0xC9,0xE4,0xF0,0xFC,0x34,0x04,0xD9,0xF8,
0xE0,0x80,0xC9,0xFA,0xCB,0xCB,0x80,0x01,0x02,0x00,0xDF,0xCB,0xCB,0x80,0x00,0x02,
0x01,0xDF,0xCB,0xCB,0x2C,0x9A,0x00,0x02,0x28,0xDF,0xF0,0xFC,0x34,0x04,0xD9,0xF8,
0xE0,0x80,0xC9,0xFA,0xCB,0xCB,0x80,0x00,0x02,0x00,0xDF,0xCB,0xCB,0x80,0x00,0x02,
0x01,0xDF,0xCB,0xCB,0x2C,0x9A,0x00,0x02,0x28,0xDF,0xF0,0xFC
};

/*
5 bset 0x10,(5A5) // FE 80 A5 05 10
2 rets // F0 FC
*/
unsigned char clr_code[] = 
{
0xFE,0x80,0xA5,0x05,0x10,0xF0,0xFC
};

unsigned short end = 0x00FF; // endian detection

int hex_atoi(void *dest, char *src, int dest_len)
{
	int src_last, i, j;
	unsigned char val;

	memset(dest, 0, dest_len);

	src_last = strlen(src) - 1;

	for(i = src_last; i >= 0; i--) {
		if(src[i] >= '0' && src[i] <= '9')
			val = src[i] - 0x30;
		else if(src[i] >= 'a' && src[i] <= 'f')
			val = (src[i] - 0x60) + 9;
		else if(src[i] >= 'A' && src[i] <= 'F')
			val = (src[i] - 0x40) + 9;
		else
			return 1; // invalid hex digit

		j = src_last - i;

		if(j & 1)
			val <<= 4;

		if(*((unsigned char *)&end)) // little endian CPU
			j >>= 1;
		else // big endian CPU
			j = dest_len - ((j >> 1) + 1);

		if(j >= dest_len || j < 0)
			break;

		((unsigned char *)dest)[j] |= val;
	}

	return 0;
}


int main(int argc, char *argv[])
{
	int fd;
	struct cdrom_generic_command cgc;
	struct request_sense sense;
	unsigned char param_list[0x10], *buffer, *ptr;
	static unsigned char txblk[0x800];
	unsigned int sum, i, sector, blocks, blklen, buffer_len, sector_size, bytes;
	FILE *fptr;

	if(argc != 5) {
		printf("usage: flashsec47 device firmware_image target_sector_address sector_size\n");
		return 1;
	}

	if((fd = open(argv[1], O_RDONLY | O_NONBLOCK)) == -1) {
		perror("open");
		return 1;
	}

	if(!(fptr = fopen(argv[2], "rb"))) {
		perror("fopen");
		close(fd);
		return 1;
	}

	if(hex_atoi(&sector_size, argv[4], sizeof(sector_size))) {
		printf("invalid sector size\n");
		close(fd);
		fclose(fptr);
		return 1;
	}

	if(hex_atoi(&sector, argv[3], sizeof(sector)) || (sector % sector_size) || sector < 0x90000000) {
		printf("invalid sector address\n");
		close(fd);
		fclose(fptr);
		return 1;
	}

	// Create our flash buffer
	if(fseek(fptr, sector - 0x90000000, SEEK_SET) == -1) {
		perror("fseek");
		close(fd);
		fclose(fptr);
		return 1;
	}

	buffer_len = (sizeof(control_code) + sizeof(erase_code) + sizeof(program_code) + sector_size + 2); // + 2 for 16-bit checksum
	if(buffer_len & 0x03)
		buffer_len += 4 - (buffer_len & 0x03); // round up to 4 byte boundary

	if(!(buffer = (unsigned char *)malloc(buffer_len))) {
		printf("malloc() failed\n");
		close(fd);
		fclose(fptr);
		return 1;
	}

	memset(buffer, 0, buffer_len);

	if(!*((unsigned char *)&end)) { // big endian CPU
		i = sector;
		((unsigned char *)&sector)[0] = ((unsigned char *)&i)[3];
		((unsigned char *)&sector)[1] = ((unsigned char *)&i)[2];
		((unsigned char *)&sector)[2] = ((unsigned char *)&i)[1];
		((unsigned char *)&sector)[3] = ((unsigned char *)&i)[0];
		((unsigned char *)&i)[0] = ((unsigned char *)&sector_size)[3];
		((unsigned char *)&i)[1] = ((unsigned char *)&sector_size)[2];
		((unsigned char *)&i)[2] = ((unsigned char *)&sector_size)[1];
		((unsigned char *)&i)[3] = ((unsigned char *)&sector_size)[0];
	}
	else
		i = sector_size;

	*((unsigned int *)&control_code[8]) = sector;
	*((unsigned int *)&control_code[30]) = sector;
	*((unsigned int *)&control_code[48]) = i; // little endian sector_size

	memcpy(buffer, control_code, sizeof(control_code));
	ptr = &buffer[sizeof(control_code)];
	memcpy(ptr, erase_code, sizeof(erase_code));
	ptr += sizeof(erase_code);
	memcpy(ptr, program_code, sizeof(program_code));
	ptr += sizeof(program_code);

	if(fread(ptr, sector_size, 1, fptr) != 1) {
		perror("fread");
		close(fd);
		fclose(fptr);
		return 1;
	}

	// Caculate buffer checksum
	for(sum = 0, i = 0; i < buffer_len - 2; i++)
		sum += buffer[i];
	sum = 0x10000 - (sum & 0x0000FFFF);
	buffer[buffer_len - 2] = (unsigned char)(sum & 0x00FF);
	buffer[buffer_len - 1] = (unsigned char)((sum & 0xFF00) >> 8);

	// Initiate modeB (required to configure upload)
	memset(&cgc, 0, sizeof(cgc));
	cgc.cmd[0] = 0xE7;
	cgc.cmd[1] = 0x48;
	cgc.cmd[2] = 0x49;
	cgc.cmd[3] = 0x54;
	cgc.cmd[4] = 0x30;
	cgc.cmd[5] = 0x90;
	cgc.cmd[6] = 0x90;
	cgc.cmd[7] = 0xD0;
	cgc.cmd[8] = 0x01;

	cgc.sense = &sense;
	cgc.data_direction = CGC_DATA_NONE;
	cgc.timeout = 15000;

	if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
		perror("ioctl");
		printf("failed to initiate modeB (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
	}
	else {
		// Stop the disc
		memset(&cgc, 0, sizeof(cgc));
		cgc.cmd[0] = 0x1B;

		cgc.sense = &sense;
		cgc.data_direction = CGC_DATA_NONE;
		cgc.timeout = 15000;

		if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1 && (sense.sense_key != 0x02 || sense.asc != 0x3A)) {
			perror("ioctl");
			printf("failed to stop the disc (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
		}
		else {
			// Set (5A5) bit 4 (required to upload code on the 0047 firmware)
			for(i = 0; i < sizeof(clr_code); i++) {
				// Hitachi poke RAM command
				memset(&cgc, 0, sizeof(cgc));
				cgc.cmd[0] = 0xE7;
				cgc.cmd[1] = 0x48;
				cgc.cmd[2] = 0x49;
				cgc.cmd[3] = 0x54;
				cgc.cmd[4] = 0xCC;
				cgc.cmd[5] = clr_code[i];
				cgc.cmd[8] = (unsigned char)(((0x80000000 + i) & 0xFF000000) >> 24); // address MSB
				cgc.cmd[9] = (unsigned char)(((0x80000000 + i) & 0x00FF0000) >> 16); // address
				cgc.cmd[10] = (unsigned char)(((0x80000000 + i) & 0x0000FF00) >> 8); // address
				cgc.cmd[11] = (unsigned char)((0x80000000 + i) & 0x000000FF); // address LSB
	
				cgc.sense = &sense;
				cgc.data_direction = CGC_DATA_NONE;
				cgc.timeout = 15000;
				
				if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
					perror("ioctl");
					printf("Hitachi poke RAM command failed on byte %u (sense: %02X/%02X/%02X)\n", i, sense.sense_key, sense.asc, sense.ascq);
					close(fd);
					fclose(fptr);
					free(buffer);
					return 1;
				}
			}

			// set (59E) bit 3, if it's not already set (required to execute code in RAM)
			memset(&cgc, 0, sizeof(cgc));
			cgc.cmd[0] = 0x55;
			cgc.cmd[1] = 0x10;
			cgc.cmd[8] = 0x08;

			memset(param_list, 0, sizeof(param_list));
			param_list[1] = 6;
		
			cgc.buffer = param_list;
			cgc.buflen = 8;
			cgc.sense = &sense;
			cgc.data_direction = CGC_DATA_WRITE;
			cgc.timeout = 15000;

			ioctl(fd, CDROM_SEND_PACKET, &cgc);

			// jump to code in RAM (this code sets (5A5) bit 4)
			memset(&cgc, 0, sizeof(cgc));
			cgc.cmd[0] = 0x55;
			cgc.cmd[1] = 0x10;
			cgc.cmd[3] = 0x48; // 'H'
			cgc.cmd[4] = 0x4C; // 'L'
			cgc.cmd[8] = 0x08;

			memset(param_list, 0, sizeof(param_list));
			param_list[1] = 6;
			
			cgc.buffer = param_list;
			cgc.buflen = 8;
			cgc.sense = &sense;
			cgc.data_direction = CGC_DATA_WRITE;
			cgc.timeout = 15000;

			ioctl(fd, CDROM_SEND_PACKET, &cgc);

			// Clear (59E) bit 3 if it's set (this bit will cause us problems if it's left set)
			memset(&cgc, 0, sizeof(cgc));
			cgc.cmd[0] = 0x55;

			cgc.sense = &sense;
			cgc.data_direction = CGC_DATA_NONE;
			cgc.timeout = 15000;

			ioctl(fd, CDROM_SEND_PACKET, &cgc);

			// Configure upload
			memset(&cgc, 0, sizeof(cgc));
			cgc.cmd[0] = 0x55;
			cgc.cmd[1] = 0x10;
			cgc.cmd[8] = 0x10;
			cgc.cmd[11] = 0x01;

			memset(param_list, 0, sizeof(param_list));
			param_list[9] = 0x06;
			param_list[10] = 0x48;
			param_list[11] = 0x4C;
			param_list[14] = (unsigned char)((buffer_len & 0xFF00) >> 8);
			param_list[15] = (unsigned char)(buffer_len & 0x00FF);

			cgc.buffer = param_list;
			cgc.buflen = 0x10;
			cgc.sense = &sense;
			cgc.data_direction = CGC_DATA_WRITE;
			cgc.timeout = 15000;

			if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
				perror("ioctl");
				printf("failed to configure upload (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
			}
			else {
				// Upload our buffer in 0x7F8 byte chunks
				blocks = buffer_len / 0x7F8;
				if(buffer_len % 0x7F8)
				blocks++;
				for(i = 0; i < blocks; i++) {
					if(i != blocks - 1)
						blklen = 0x800;
					else
						blklen = (buffer_len % 0x7F8) + 8;

					memset(&cgc, 0, sizeof(cgc));
					cgc.cmd[0] = 0x55;
					cgc.cmd[1] = 0x10;
					cgc.cmd[7] = (unsigned char)((blklen & 0xFF00) >> 8);
					cgc.cmd[8] = (unsigned char)(blklen & 0x00FF);
					cgc.cmd[11] = 0x01;

					memset(txblk, 0, sizeof(txblk));
					txblk[4] = 0x48;
					txblk[5] = 0x4C;

					memcpy(&txblk[8], &buffer[i * 0x7F8], blklen - 8);

					cgc.buffer = txblk;
					cgc.buflen = blklen;
					cgc.sense = &sense;
					cgc.data_direction = CGC_DATA_WRITE;
					cgc.timeout = 15000;

					if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
						perror("ioctl");
						printf("failed to upload buffer block %u (sense: %02X/%02X/%02X)\n", i, sense.sense_key, sense.asc, sense.ascq);
						close(fd);
						fclose(fptr);
						free(buffer);
						return 1;
					}
				}

				// Execute flasher code
				memset(&cgc, 0, sizeof(cgc));
				cgc.cmd[0] = 0x55;
				cgc.cmd[1] = 0x10;
				cgc.cmd[3] = 0x48;
				cgc.cmd[4] = 0x4C;
				cgc.cmd[6] = 0x01;
				cgc.cmd[11] = 0x01;

				cgc.sense = &sense;
				cgc.data_direction = CGC_DATA_NONE;
				cgc.timeout = 15000;

				if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
					perror("ioctl");
					printf("failed to execute flasher (sense: %02X/%02X/%02X)\n", i, sense.sense_key, sense.asc, sense.ascq);
				}
				else
					printf("done\n");
			}
		}
	}

	close(fd);
	fclose(fptr);
	free(buffer);

	printf("\n");

	return 0;
}

