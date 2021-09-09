all: assemble link

assemble:
	mn10300-elf32-as -o $(FILENAME).o $(FILENAME).s
link:
	mn10300-elf32-ld -T hitachi.ld $(FILENAME).o -o $(FILENAME).bin
