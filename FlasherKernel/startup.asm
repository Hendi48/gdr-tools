; This is the actual entry point of the kernel.
; The switch into the 32-bit 'Protected Mode' has already been performed
; (by the boot loader).
; The assembly code just performs the absolute necessary steps (like setting up
; the stack) to be able to jump into the C++ code -- and continue further
; initialization in a (more) high-level language.

;
;   Constants
;

; Stack for the main function
STACKSIZE: equ 65536

; Base address for graphics memory
CGA: equ 0xB8000

; 254 GB max RAM size for page table
MAX_MEM: equ 254

; Multiboot constants
MULTIBOOT_PAGE_ALIGN     equ   1<<0
MULTIBOOT_MEMORY_INFO    equ   1<<1
MULTIBOOT_HEADER_MAGIC   equ   0x1badb002

; Multiboot flags (ELF-specific!)
MULTIBOOT_HEADER_FLAGS   equ   MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_HEADER_CHKSUM  equ   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
MULTIBOOT_EAX_MAGIC      equ   0x2badb002

[GLOBAL pagetable_start]
pagetable_start:  equ 0x103000

[GLOBAL pagetable_end]
pagetable_end:  equ 0x200000

;
;   System
;

; Functions provided by us
[GLOBAL startup]
[GLOBAL idt]
[GLOBAL __cxa_pure_virtual]
[GLOBAL _ZdlPv]
[GLOBAL _ZdlPvj]
[GLOBAL _ZdlPvm]

; Functions defined in C
[EXTERN main]
[EXTERN guardian]

; Symbols defined by the compiler
[EXTERN ___BSS_START__]
[EXTERN ___BSS_END__]
[EXTERN __init_array_start]
[EXTERN __init_array_end]
[EXTERN __fini_array_start]
[EXTERN __fini_array_end]

[SECTION .text]

;
;   System startup, part 1 (32-bit Protected mode)
;
;   Initialize GDT and page table and switch to 64-bit Long mode.
;

[BITS 32]

; Traditional entrypoint
	jmp    startup
	align  4        ; 32-bit alignment

;
;   Multiboot header
;

	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_HEADER_CHKSUM
	dd 0 ; header_addr (ignored)
	dd 0 ; load_addr (ignored)
	dd 0 ; load_end_addr (ignored)
	dd 0 ; bss_end_addr (ignored)
	dd 0 ; entry_addr (ignored)
	dd 0 ; mode_type (ignored)
	dd 0 ; width (ignored)
	dd 0 ; height (ignored)
	dd 0 ; depth (ignored)

;
;  GRUB entrypoint
;

startup:
	cld              ; GCC expects DF to be cleared
	cli              ; Disable interrupts
	lgdt   [gdt_80]  ; Load new segment descriptors

	; Global data segment
	mov    eax, 3 * 0x8
	mov    ds, ax
	mov    es, ax
	mov    fs, ax
	mov    gs, ax

	; Setup initial stack
	mov    ss, ax
	mov    esp, init_stack+STACKSIZE

;
;  Switch to Long mode
;

init_longmode:
	; Enable PAE
	mov    eax, cr4
	or     eax, 1 << 5
	mov    cr4, eax

	; Setup page table
	call   setup_paging

	; Enable long mode (in compatibility mode for the time being)
	mov    ecx, 0x0C0000080 ; EFER (Extended Feature Enable Register)
	rdmsr
	or     eax, 1 << 8 ; LME (Long Mode Enable)
	wrmsr

	; Activate paging
	mov    eax, cr0
	or     eax, 1 << 31
	mov    cr0, eax

	; Jump to 64 bit code segment (this fully enables long mode)
	jmp    2 * 0x8 : longmode_start

;
;   Set up a (provisional) paeg table with 2 MB page size, which maps
;   the first MAX_MEM GB directly to physical memory.
;   This is required because a functional page table is required for long mode.
;   The system currently must not have more RAM than that.
;

setup_paging:
	; PML4 (Page Map Level 4 / 1st stage)
	mov    eax, pdp
	or     eax, 0xf
	mov    dword [pml4+0], eax
	mov    dword [pml4+4], 0

	; PDPE (Page-Directory-Pointer Entry / 2nd stage) for 16GB
	mov    ecx, 0
	mov    eax, pd
	or     eax, 0x7           ; Address of the first table (3rd stage) with flags
fill_tables2:
	cmp    ecx, MAX_MEM       ; Reference MAX_MEM tables
	je     fill_tables2_done
	mov    dword [pdp + 8*ecx + 0], eax
	mov    dword [pdp + 8*ecx + 4], 0
	add    eax, 0x1000        ; Each table is 4kB
	inc    ecx
	ja     fill_tables2
fill_tables2_done:

	; PDE (Page Directory Entry / 3rd stage)
	mov    ecx, 0
	mov    eax, 0x0 | 0x87    ; Start address 0 + Flags
fill_tables3:
	cmp    ecx, 512*MAX_MEM   ; Fill MAX_MEM tables with 512 entries each
	je     fill_tables3_done
	mov    dword [pd + 8*ecx + 0], eax
	mov    dword [pd + 8*ecx + 4], 0
	add    eax, 0x200000      ; 2 MB per page
	inc    ecx
	ja     fill_tables3
fill_tables3_done:

	; Set base pointer to PML4
	mov    eax, pml4
	mov    cr3, eax
	ret

;
;   System startup, part 2 (64-bit Long mode)
;
;   Clear the BSS segment and initialize the IDT and PICs.
;   Then execute the C/C++ constructors and finally call main().
;

longmode_start:
[BITS 64]
	; Zero BSS section
	mov    rdi, ___BSS_START__
clear_bss:
	mov    byte [rdi], 0
	inc    rdi
	cmp    rdi, ___BSS_END__
	jne    clear_bss

	; Initialize IDT and PICs
	call   setup_idt
	call   reprogram_pics
	call   setup_cursor

	fninit         ; Initialize FPU
	call   _init   ; Run constructors of global objects
	call   main    ; Call C/C++ kernel
	call   _fini   ; Run destruktoren
	cli            ; We shouldn't reach this point
	hlt

;
;   Interrupt handling
;

; Macro that generates an individual wrapper for each interrupt
%macro wrapper 1
wrapper_%1:
	push   rbp
	mov    rbp, rsp
	push   rax
	mov    al, %1
	jmp    wrapper_body
%endmacro

%assign i 0
%rep 256
wrapper i
%assign i i+1
%endrep

; Shared body
wrapper_body:
	; Expected by GCC
	cld
	; Save volatile registers
	push   rcx
	push   rdx
	push   rdi
	push   rsi
	push   r8
	push   r9
	push   r10
	push   r11

	; Wrapper only sets lower 8 bits
	and    rax, 0xff

	; Pass interrupt number as argument
	mov    rdi, rax
	mov    rsi, rsp
	add    rsi, 12*8
	call   guardian

	; Restore volatile registers
	pop    r11
	pop    r10
	pop    r9
	pop    r8
	pop    rsi
	pop    rdi
	pop    rdx
	pop    rcx

	; ...including the ones from the wrapper
	pop    rax
	pop    rbp

	; Done!
	iretq

;
; Relocates the IDT entries and set the IDTR
;

setup_idt:
	mov    rax, wrapper_0

	; Bits 0..15 -> ax, 16..31 -> bx, 32..64 -> edx
	mov    rbx, rax
	mov    rdx, rax
	shr    rdx, 32
	shr    rbx, 16

	mov    r10, idt   ; Pointer to the current interrupt gate
	mov    rcx, 255   ; Counter
.loop:
	add    [r10+0], ax
	adc    [r10+6], bx
	adc    [r10+8], edx
	add    r10, 16
	dec    rcx
	jge    .loop

	lidt   [idt_descr]
	ret

;
; Make cursor blink (GRUB disables this)
;

setup_cursor:
	mov al, 0x0a
	mov dx, 0x3d4
	out dx, al
	call delay
	mov dx, 0x3d5
	in al, dx
	call delay
	and al, 0xc0
	or al, 14
	out dx, al
	call delay
	mov al, 0x0b
	mov dx, 0x3d4
	out dx, al
	call delay
	mov dx, 0x3d5
	in al, dx
	call delay
	and al, 0xe0
	or al, 15
	out dx, al
	ret

;
; Reprogram PICs so that all 15 hardware interrupts are located
; sequentially in the IDT
;

reprogram_pics:
	mov    al, 0x11   ; ICW1: Initialization in cascade mode
	out    0x20, al
	call   delay
	out    0xa0, al
	call   delay
	mov    al, 0x20   ; ICW2 Master: IRQ # offset (32)
	out    0x21, al
	call   delay
	mov    al, 0x28   ; ICW2 Slave: IRQ # offset (40)
	out    0xa1, al
	call   delay
	mov    al, 0x04   ; ICW3 Master: There's a slave PIC at IRQ2
	out    0x21, al
	call   delay
	mov    al, 0x02   ; ICW3 Slave: Connected to IRQ2 of master
	out    0xa1, al
	call   delay
	mov    al, 0x01   ; ICW4: 8086 mode and manual EOI
	out    0x21, al
	call   delay
	out    0xa1, al
	call   delay

	; Mask all interrupts except IRQ2 (slave)
	mov    al, 0xff
	out    0xa1, al
	call   delay
	mov    al, 0xfb
	out    0x21, al

	ret

;
; Run global constructors
;

_init:
	mov    rbx, __init_array_start
_init_loop:
	cmp    rbx, __init_array_end
	je     _init_done
	mov    rax, [rbx]
	call   rax
	add    rbx, 8
	ja     _init_loop
_init_done:
	ret

;
; Run global destructors
;

_fini:
	mov    rbx, __fini_array_start
_fini_loop:
	cmp    rbx, __fini_array_end
	je     _fini_done
	mov    rax, [rbx]
	call   rax
	add    rbx, 8
	ja     _fini_loop
_fini_done:
	ret

;
; Short delay for in/out instructions
;

delay:
	jmp    .L2
.L2:
	ret

;
; Functions required by the C++ compiler. Since we don't have any
; memory management to speak of, they are empty.
;

__cxa_pure_virtual: ; "virtual" method without implementation called
_ZdlPv:             ; void operator delete(void*)
_ZdlPvj:            ; void operator delete(void*, unsigned int) for g++ 6.x
_ZdlPvm:            ; void operator delete(void*, unsigned long) for g++ 6.x
	ret

[SECTION .data]

;
; Segment descriptors
;

gdt:
	dw  0,0,0,0   ; NULL descriptor

	; 32 bit code segment descriptor
	dw  0xFFFF    ; 4Gb - (0x100000*0x1000 = 4Gb)
	dw  0x0000    ; base address=0
	dw  0x9A00    ; code read/exec
	dw  0x00CF    ; granularity=4096, 386 (+5th nibble of limit)

	; 64 bit code segment descriptor
	dw  0xFFFF    ; 4Gb - (0x100000*0x1000 = 4Gb)
	dw  0x0000    ; base address=0
	dw  0x9A00    ; code read/exec
	dw  0x00AF    ; granularity=4096, 386 (+5th nibble of limit), long mode

	; Data segment descriptor
	dw  0xFFFF    ; 4Gb - (0x100000*0x1000 = 4Gb)
	dw  0x0000    ; base address=0
	dw  0x9200    ; data read/write
	dw  0x00CF    ; granularity=4096, 386 (+5th nibble of limit)

gdt_80:
	dw  4*8 - 1   ; GDT limit (4 entries)
	dq  gdt       ; Address of GDT

;
; Interrupt Descriptor Table with 256 entries
;

idt:
%macro idt_entry 1
	dw  (wrapper_%1 - wrapper_0) & 0xffff ; Offset 0 .. 15
	dw  0x0000 | 0x8 * 2 ; Selector points to 64 bit code segment descriptor in GDT
	dw  0x8e00 ; 8 -> interrupt is present, e -> 80386 32-bit interrupt gate
	dw  ((wrapper_%1 - wrapper_0) & 0xffff0000) >> 16 ; Offset 16 .. 31
	dd  ((wrapper_%1 - wrapper_0) & 0xffffffff00000000) >> 32 ; Offset 32..63
	dd  0x00000000 ; Reserved
%endmacro

%assign i 0
%rep 256
idt_entry i
%assign i i+1
%endrep

idt_descr:
	dw  256*8 - 1    ; 256 entries
	dq idt

[SECTION .bss]

[GLOBAL MULTIBOOT_FLAGS]
[GLOBAL MULTIBOOT_LOWER_MEM]
[GLOBAL MULTIBOOT_UPPER_MEM]
[GLOBAL MULTIBOOT_BOOTDEVICE]
[GLOBAL MULTIBOOT_CMDLINE]
[GLOBAL MULTIBOOT_MODULES_COUNT]
[GLOBAL MULTIBOOT_MODULES_ADDRESS]

MULTIBOOT_FLAGS:            resd 1
MULTIBOOT_LOWER_MEM:        resd 1
MULTIBOOT_UPPER_MEM:        resd 1
MULTIBOOT_BOOTDEVICE:       resd 1
MULTIBOOT_CMDLINE:          resd 1
MULTIBOOT_MODULES_COUNT:    resd 1
MULTIBOOT_MODULES_ADDRESS:  resd 1

global init_stack:data (init_stack.end - init_stack)
init_stack:
	resb STACKSIZE
.end:

[SECTION .global_pagetable]

[GLOBAL pml4]
[GLOBAL pdp]
[GLOBAL pd]

pml4:
	resb   4096
	alignb 4096

pdp:
	resb   MAX_MEM*8
	alignb 4096

pd:
	resb   MAX_MEM*4096
