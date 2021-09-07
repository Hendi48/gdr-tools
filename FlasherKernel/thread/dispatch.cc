#include "thread/dispatch.h"

void Dispatcher::go(Coroutine& first) {
	m_life = &first;
	m_life->go();
}

void Dispatcher::dispatch(Coroutine& next) {
	Coroutine* old_life = active();
	m_life = &next;
	old_life->resume(next);
}

Coroutine* Dispatcher::active() {
	return m_life;
}
