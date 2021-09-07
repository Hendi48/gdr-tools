#include "thread/scheduler.h"
#include "sync/waitingroom.h"
#include "device/cgastr.h"
#include "machine/cpu.h"
 
void Scheduler::ready(Thread& that) {
	m_ready.enqueue(&that);
}

void Scheduler::schedule() {
	Thread* thread = (Thread*) m_ready.dequeue();
	if (thread) {
		go(*thread);
	} else {
		kout << "Schedule fault" << endl;
		kout.flush();
		cpu.halt();
	}
}

void Scheduler::exit() {
	Thread* next = (Thread*) m_ready.dequeue();
	if (next) {
		dispatch(*next);
	} else {
		kout << "Exit fault" << endl;
		kout.flush();
		cpu.halt();
	}
}

void Scheduler::kill(Thread& that) {
    Waitingroom* room = that.waiting_in();
    if (room) {
        room->remove(&that);
    } else {
		// Regular kill
		if (active() == &that)
			exit();
		else
			m_ready.remove(&that);
	}
}

void Scheduler::resume() {
	Thread* current = (Thread*) active();
	m_ready.enqueue(current);
	Thread* next = (Thread*) m_ready.dequeue();
	if (!next) {
		kout << "Resume fault" << endl;
		kout.flush();
		cpu.halt();
	}

	if (current != next) {
		dispatch(*next);
	} else {
		asm volatile ("pause");
	}
}

void Scheduler::block(Waitingroom& waitingroom)
{
    Thread* thread = (Thread*) active();
    thread->waiting_in(&waitingroom);
    waitingroom.enqueue(thread);
    Scheduler::exit();
}

void Scheduler::wakeup(Thread& thread)
{
    thread.waiting_in(nullptr);
    ready(thread);
}
