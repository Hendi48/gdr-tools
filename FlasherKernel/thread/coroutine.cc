#include "thread/coroutine.h"
#include "machine/toc.h"
#include "thread/kickoff.h"

Coroutine::Coroutine(void* tos) {
	toc_settle(&m_regs, tos, (toc_kickoff_t) &kickoff); // kickoff shall be called
}

void Coroutine::go() {
	toc_go(&m_regs, this);
}

void Coroutine::resume(Coroutine& next) {
	toc_switch(&m_regs, &next.m_regs, &next);
}
