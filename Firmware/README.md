# Firmware binaries

.bin files are plaintext firmware without header, while .dld files ("download") are what the official firmware flashing tools process. They have a 64 bytes header specifying a checksum, the drive they're intended for and some more info, followed by 256K encrypted firmware, followed by an unencrypted flasher stub that is uploaded into the drive cache to do the actual flashing (this is because the drive can't be executing from flash while flashing itself).

* 3120L_0046: Original 3120L firmware
* 8050L_0012: 8050L firmware for cross-flashing an 8163B drive. Drives with this FW work with the original MS dashboard
* 8050L_0012_patched: Same as 8050L_0012 but patched in one place to allow reading all memory areas using the 0xE7 memdump command
* 8163B_0M26: Original firmware dumped from an OEM (IBM?) GDR-8163B drive
* 8164B_0L06_mod: Patched 8164B firmware that auto-switches to the Xbox partition when inserting an Xbox Game Disc. Very useful in PCs or Xbox with custom dashboard; does not work with original MS dashboard because it lacks the challenge/response mechanism
* 8164B_0L06_orig: Original 8164B firmware for LG drives
* PrepStub: Flasher stub (from the end of a DLD) as standalone binary
