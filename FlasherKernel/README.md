# FlasherKernel

This is a tiny 64-bit OS kernel (based on a student operating system) that has been adapted for talking to MN103-based Hitachi drives via a PCI/PCI-E IDE controller.

It is especially suited for recovering drives after a bad flash (or if, like me, you wrote to flash and forgot about the checksum). It also contains some Xbox-related functions if you have a drive with the 8050L firmware.

It has been tested with LG GDR-8163B and LG GDR-8164B drives on Marvell controllers.

For a drive in recovery mode, it can take 1 to 10 minutes for your IDE controller to give up detecting the drive when booting, depending on how much the controller sucks. Once you've booted this kernel, it will be able to talk to the seemingly dead drive just fine.

## Building

Configure the appropriate PCI bus/slot/function for your target system's IDE controller in `user/appl.cc`. You can get these details using `lspci -v` under Linux. USB-connected controllers **will not** work.

An ISO build requires GRUB. If you don't need an ISO, you can use `make build/system`.

For Debian-based systems:
```apt install --no-install-recommends grub-common xorriso grub-pc-bin```

Build the kernel with `make`, then flash the ISO to a USB drive. Alternatively copy the `system` file onto a drive with a bootloader (e.g., GRUB) and configure the bootloader to launch `system` using multiboot.

The kernel supports Undefined Behavior Sanitizer. Use `SANFLAGS=-fsanitize=undefined make` to build the kernel with UBSAN.

For additional targets (such as QEMU), check the Makefile.

## Usage

After startup, FlasherKernel will ask you to select a drive (0/1 for master/slave) on the IDE bus or do a software reset on the controller (r). When you select a drive, it will send a SCSI `INQUIRY` packet and attempt to identify the drive.

If it fails, you are prompted for selection again. Otherwise you are dropped into a very rudimentary command shell.

## Commands

All address/data/size parameters are expected in hexadecimal notation without prefix.

If your drive is in recovery mode, the firmware doesn't have any disc reading logic and only the "Recovery mode only" commands will work. They upload custom machine code to the drive and execute it.

### General
`m <address> <size>`: Read arbitrary memory using Hitachi debug command.

`c`: Read capacity of disc.

`s <lba> <size>`: Read sector at the given LBA from disc. Size must range from `0` to `800`.

### Recovery mode only
`i`: Identify the flash chip in your drive. The vendor will be returned in "CountLo" and the chip ID in "CountHi".

`r <address>`: Read 4 bytes from memory (returned in ATA task file registers). If you want to inspect flash, you want to specify an address between `90000000` and `9003FFFC` here.

`w <address> <byte>`: Write 1 byte to the given address.
**Important**: You need to encrypt the byte, else the chip will read back gibberish. Also note that due to the encryption being done on 32-bit values, writing 1 byte will affect a whole dword. Flash doesn't allow you to set bits that are 0 back to 1 without doing an erase.

`e <address> <size>`: Erase the sector at the given address. Size is only used to verify that erasing worked. 
**Important**: The sector layout is determined by the specs of the flash chip in your drive, so be sure to read the datasheet first!

`f <address>`: Flash a sector to the drive. The data needs to be put into `codeblobs.h` at compile time.

These commands could theoretically also work in non-recovery mode if one sends an unlock command beforehand ("Mode B"). Check the sources for the Linux programs in the parent directory.

### Xbox Game Discs
`a`: Authenticate the disc by performing a special challenge/response protocol with the drive. Once complete, the drive will switch to the Xbox partition.

`n`: Send an ATAPI nop (`0x00`) command to the drive. Hitachi inserted a backdoor there which will immediately make the drive set itself to authenticated and switch to the Xbox partition.

## Drive Memory Map

```
0-10000: RAM with some memory-mapped IO sprinkled in the higher ranges
80000000-80040000: 256K drive cache
90000000-90040000: 256K flash
```

## Misc Notes

ATA interrupt notifications are currently printed directly in `ATADevice::prologue()` without any queueing/synchronization, which can lead to slightly garbled screen output at times. If this gets annoying and you don't need the details provided by them, you can comment the two lines responsible for them.
