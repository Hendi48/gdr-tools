#pragma once

typedef void (*toc_kickoff_t)(void*);

/*
 * The toc structure is used to store non-volatile registers during a coroutine
 * switch.
 * The GNU C compiler uses the System V AMD64 ABI, which declares RBP, RBX and
 * R12 through R15 as non-volatile registers, which need to be saved manually
 * during context switches. Furthermore there are 108 bytes of non-volatile
 * extended CPU state, which is only important when using FPU/SIMD instructions.
 * 
 * Note: toc.inc contains names for the structure offsets in toc, so if you
 * change toc.h, you also need to change toc.inc (and vice-versa).
 */
struct toc {
	void *rbx;
	void *r12;
	void *r13;
	void *r14;
	void *r15;
	void *rbp;
	void *rsp;
	char fpu[108]; // Optional: 108 Byte extended CPU state (MMX, SSE, ...)
};

#ifdef __cplusplus
extern "C" {
#endif
	void toc_settle (struct toc* regs, void* tos, toc_kickoff_t kickoff);
	void toc_go(struct toc* regs, void* coroutine);
	void toc_switch(struct toc* regs_now, struct toc* regs_then, void* coroutine);
#ifdef __cplusplus
}
#endif
