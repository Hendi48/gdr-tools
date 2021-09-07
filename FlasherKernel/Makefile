# make: Compile system and build iso
# make build/system: Compile system only
# make bootdisk: Write system image to a device (e.g., USB drive)
# make clean: Cleanup build dirs
# make qemu/qemu-gdb: Start QEMU (optionally with GDB)
# make gdb/ddd: Start gdb/ddd and connect to GDB stub

# -----------------------------------------------------------------------------
# Gather source files:

STARTUP_SOURCE = ./startup.asm
CC_SOURCES = $(shell find . -name "*.cc")
C_SOURCES = $(shell find . -name "*.c")
ASM_SOURCES = $(shell find ./machine -name "*.asm")

# Variables:
#    ASMOBJFORMAT: Object format for assembler. Must match the format
#                  the C compiler emits so that files can be linked
#    OBJDIR:       File for object files
#    DEPDIR:       File for dependency files
#    DRIVE_HD:     Device name for bootdisk target (care advised!)
#    DELETE:       rm command
#    ASM:          Assembler program (nasm)
#    CC/CXX:       C/C++ compiler
#    CFLAGS:       Flags for C compiler
#    CXXFLAGS:     Flags for C++ compiler
#    QEMU:         Name of architecture-specific QEMU executable
#    MKRESCUE:     Name of GRUB mkrescue program

VERBOSE = @
ASMOBJFORMAT = elf64
OBJDIR = ./build
DEPDIR = ./dep
# WARNING: The filesystem on the specified device will be overwritten!
DRIVE_HD = /dev/sdz
DELETE = rm
ASM = nasm
QEMU ?= qemu-system-x86_64
CC ?= gcc
CXX ?= g++
CFLAGS := $(CFLAGS) -m64 -mno-red-zone -Wall -Wno-write-strings -fno-stack-protector -nostdlib -I. -g $(SANFLAGS) #-DDEBUG
CXXFLAGS := $(CFLAGS) -Wno-non-virtual-dtor -ffreestanding -fno-threadsafe-statics -fno-use-cxa-atexit -fno-rtti -fno-exceptions

ifneq ($(shell which grub-mkrescue 2> /dev/null),)
MKRESCUE = grub-mkrescue
endif
ifneq ($(shell which grub2-mkrescue 2> /dev/null),)
MKRESCUE = grub2-mkrescue
endif

# -------------------------------------------------------------------------
# Names of source directories

VPATH = $(sort $(dir $(STARTUP_SOURCE) $(CC_SOURCES) $(C_SOURCES) $(ASM_SOURCES)))

# -------------------------------------------------------------------------
# Lists of object files

FIRST_OBJECT = $(addprefix $(OBJDIR)/,$(patsubst %.asm,_%.o, $(notdir $(STARTUP_SOURCE))))
C_OBJECTS = $(notdir $(C_SOURCES:.c=.o))
CC_OBJECTS = $(notdir $(CC_SOURCES:.cc=.o))

DEP_FILES = $(patsubst %.o,$(DEPDIR)/%.d,$(C_OBJECTS))
DEP_FILES += $(patsubst %.o,$(DEPDIR)/%.d,$(CC_OBJECTS))

ASM_OBJECTS = $(patsubst %.asm,_%.o, $(notdir $(ASM_SOURCES)))
OBJPRE = $(addprefix $(OBJDIR)/,$(ASM_OBJECTS) $(C_OBJECTS) $(CC_OBJECTS))

# --------------------------------------------------------------------------
# Default target: ISO image (for qemu or an actual boot disk)

all: $(OBJDIR)/bootdisk.iso

# --------------------------------------------------------------------------
# Rules for creating dependencies

$(DEPDIR)/%.d : %.c
	@echo "DEP		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) $(CFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

$(DEPDIR)/%.d : %.cc
	@echo "DEP		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) $(CXXFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

# --------------------------------------------------------------------------
# Rules for creating object files

$(OBJDIR)/%.o : %.c
	@echo "CC		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.cc
	@echo "CXX		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJDIR)/_%.o : %.asm
	@echo "ASM		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(ASM) -f $(ASMOBJFORMAT) -o $@ $<

# --------------------------------------------------------------------------
# Link system

$(OBJDIR)/system: $(FIRST_OBJECT) $(OBJPRE)
	@echo "LD		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) $(CXXFLAGS) -static -z max-page-size=0x1000 -e startup -T sections -o $(OBJDIR)/system $(FIRST_OBJECT) $(OBJPRE)

# --------------------------------------------------------------------------
# 'bootdisk.iso' consists of the system and a bootloader (GRUB)
# The system is started via multi-boot so that it doesn't need to take
# care of switching to protected mode or initialization after BIOS.

$(OBJDIR)/bootdisk.iso : $(OBJDIR)/system
	@echo "BUILD		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) cp $< isofiles/boot/system
	$(VERBOSE) $(MKRESCUE) -o $@ isofiles

# --------------------------------------------------------------------------

clean:	
	@echo "RM		$(OBJDIR)"
	$(VERBOSE) rm -rf $(OBJDIR)
	@echo "RM		$(DEPDIR)"
	$(VERBOSE) rm -rf $(DEPDIR)

# --------------------------------------------------------------------------

bootdisk: $(OBJDIR)/bootdisk.iso
	@echo "CP		$<"
	$(VERBOSE) cp $< $(DRIVE_HD)

# --------------------------------------------------------------------------
# 'qemu' runs the system under QEMU

qemu: $(OBJDIR)/bootdisk.iso
	'$(QEMU)' -drive file=build/bootdisk.iso,format=raw -k en-us -no-reboot -no-shutdown

# --------------------------------------------------------------------------
# 'qemu-smp' runs QEMU with SMP support and 2 CPUs.

qemu-smp: $(OBJDIR)/bootdisk.iso
	'$(QEMU)' -drive file=build/bootdisk.iso,format=raw -k en-us -smp 2

# --------------------------------------------------------------------------

qemu-gdb: $(OBJDIR)/bootdisk.iso
	$(VERBOSE) echo "target remote localhost:$(shell echo $$(( $$(id -u) + 1023 )))" > /tmp/gdbcommands.$(shell id -u)
	'$(QEMU)' -drive file=build/bootdisk.iso,format=raw -k en-us -gdb tcp::$(shell echo $$(( $$(id -u) + 1023 )))

# --------------------------------------------------------------------------
# 'gdb' starts the gdb debugger and connects to stub running inside QEMU
# Note: gdb currently (2017) doesn't support systems that switch
# from protected mode to long mode at runtime. GDB must thus only be
# started once the change to long mode has been performed.
# Starting qemu via `qemu -S` is not possible.

gdb:
	gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

# --------------------------------------------------------------------------

ddd:
	ddd --gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

# --------------------------------------------------------------------------
# Include dependencies

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP_FILES)
endif

.PHONY: clean bootdisk gdb ddd qemu qemu-smp qemu-gdb
