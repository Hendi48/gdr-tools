#include "syscall/guarded_scheduler.h"
#include "interrupt/secure.h"

void Guarded_Scheduler::ready(Thread& that) {
	Secure sec;
	Scheduler::ready(that);
}

void Guarded_Scheduler::exit() {
	Secure sec;
	Scheduler::exit();
}

void Guarded_Scheduler::kill(Thread& that) {
	Secure sec;
	Scheduler::kill(that);
}

void Guarded_Scheduler::resume() {
	Secure sec;
	Scheduler::resume();
}

Guarded_Scheduler scheduler;
