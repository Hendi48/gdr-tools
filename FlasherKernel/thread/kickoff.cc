#include "interrupt/guard.h"
#include "thread/coroutine.h"
#include "thread/kickoff.h"
#include "syscall/guarded_scheduler.h"

void kickoff(Coroutine *object) {
	guard.leave();
	object->action();
	scheduler.exit();
}
