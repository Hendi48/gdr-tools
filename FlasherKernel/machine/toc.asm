%include "machine/toc.inc"

[GLOBAL toc_switch]
[GLOBAL toc_go]

[SECTION .text]

; TOC_GO : Starts the very first process.
;
; C prototype: void toc_go (struct toc* regs, void *coroutine);

; rdi <- regs
; rsi <- coroutine
toc_go:
mov rbx, [rdi+rbx_offset]
mov r12, [rdi+r12_offset]
mov r13, [rdi+r13_offset]
mov r14, [rdi+r14_offset]
mov r15, [rdi+r15_offset]
mov rbp, [rdi+rbp_offset]
mov rsp, [rdi+rsp_offset]
mov rdi, rsi ; Parameter coroutine for kickoff
ret

; TOC_SWITCH : Context switch. The current register set is saved
;              and the register set of the new "thread of control"
;              is loaded into the processor.
;
; C prototype: void toc_switch (struct toc* regs_now,
;                              struct toc* reg_then,
;                              void *coroutine);

toc_switch:
; save context into regs_now
mov [rdi+rbx_offset], rbx
mov [rdi+r12_offset], r12
mov [rdi+r13_offset], r13
mov [rdi+r14_offset], r14
mov [rdi+r15_offset], r15
mov [rdi+rbp_offset], rbp
mov [rdi+rsp_offset], rsp
; restore context from reg_then
mov rbx, [rsi+rbx_offset]
mov r12, [rsi+r12_offset]
mov r13, [rsi+r13_offset]
mov r14, [rsi+r14_offset]
mov r15, [rsi+r15_offset]
mov rbp, [rsi+rbp_offset]
mov rsp, [rsi+rsp_offset]
mov rdi, rdx ; Parameter coroutine for kickoff
ret
