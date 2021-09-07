[GLOBAL int_enable]
[GLOBAL int_disable]
[GLOBAL cpu_idle]
[GLOBAL cpu_halt]

[SECTION .text]

int_enable:
	sti
	ret

int_disable:
	cli
	ret

cpu_idle:
	sti ; STI and HLT are executed atomically
	hlt
    ret

cpu_halt:
	cli
	hlt
