[GLOBAL outb]
[GLOBAL outw]
[GLOBAL outl]
[GLOBAL inb]
[GLOBAL inw]
[GLOBAL inl]

[SECTION .text]
	
; OUTB: Output a byte to an I/O port.
;
;       C prototype: void outb (int port, int value);

outb:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	mov    rax, rsi
	out    dx, al
	pop    rbp
	ret

; OUTW: Output a word to an I/O port.
;
;       C prototype: void outw (int port, int value);

outw:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	mov    rax, rsi
	out    dx, ax
	pop    rbp
	ret

; OUTL
;       C prototype: void outl (int port, int value);

outl:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	mov    rax, rsi
	out    dx, eax
	pop    rbp
	ret

; INB: Read in a byte from an I/O port.
;
;      C prototype: int inb (int port);

inb:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	xor    rax, rax
	in     al, dx
	pop    rbp
	ret

; INW: Read in a word from an I/O port.
;
;      C prototype: int inw (int port);

inw:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	xor    rax, rax
	in     ax, dx
	pop    rbp
	ret

; INL
;      C prototype: int inl (int port);

inl:
	push   rbp
	mov    rbp, rsp
	mov    rdx, rdi
	xor    rax, rax
	in     eax, dx
	pop    rbp
	ret
