# Windows Flashers

Executables in this folder can outfit your DVD-ROM with a new firmware, straight from Windows! They're adapted versions of HL-DT-ST's official FWFLASH application.

Note: For safety reasons, the flasher disables any mouse input while flashing is in progress.

If you're interested in modifying such an application yourself, look for `_DLDAREA_` in a hex editor. This is followed by 3 null bytes, followed by a big endian dword that should always be 269352 (size of the following DLD), followed by the DLD content. So you can replace those 269352 bytes with another DLD in order to create a flasher that flashes a different firmware. Note that if your DLD has a wrong checksum, the flasher will silently exit upon launch without showing anything.

* 8050patcher.exe: Flashes any drive that's running a 8050L firmware with a patched 8050L firmware (8050L_0012_patched)
* 8163to8050flasher.exe: Cross-flashes your GDR-8163B with the 8050L firmware (8050L_0012)
* 8164_LL06ORIG.exe: Flashes the original 8164B firmware to your GDR-8164B (8164B_0L06_orig)
* 8164flasher.exe: Flashes a patched 8164B firmware to your GDR-8164B (8164B_0L06_mod)

Take a look at the Firmware folder in this repo for more details on the individual firmwares.

If you're having trouble getting your 8163B recognized by OSes after cross-flashing it to an 8050L, I found that ejecting the tray right on boot helps (so that the controller closes it while detecting/resetting the drive).
