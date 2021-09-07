#include "user/idle.h"
#include "syscall/guarded_scheduler.h"
#include "machine/cpu.h"

void IdleThread::action()
{
	while (1) {
		cpu.idle();
		scheduler.resume();
	}
}
