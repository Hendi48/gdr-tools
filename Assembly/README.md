# MN103 assembly code

These are the sources for the code blobs used in FlasherKernel.

## Toolchain

Download and extract any version of binutils, then build it:

```bash
mkdir build
mkdir mn103-toolchain
cd build
../binutils-2.32/configure --target mn10300-elf32 --disable-nls --prefix ../mn103-toolchain
make -j8
make install-strip
```

`mn10300-elf32-as` and `mn10300-elf32-ld` will be in `mn103-toolchain/bin`.

## Assemble

```
FILENAME=write_sector make
```

You'll get a (useless) ELF object (.o) and raw assembled code (.bin).

Note that you either need to add an absolute path to the Makefile or add `mn103-toolchain/bin` to PATH temporarily.
