#include "interrupt/guard.h"
#include "syscall/guarded_keyboard.h"
#include "syscall/guarded_scheduler.h"

#include "device/watch.h"

#include "user/appl.h"
#include "user/idle.h"


Flasher app;
IdleThread idle_thread;

//Watch watch(20000);

int main() {
	scheduler.ready(app);
	scheduler.ready(idle_thread);
	guard.enter();

	keyboard.plugin();
	cpu.enable_int();

	//watch.plugin(); disable preemption
	scheduler.schedule();

	while (1) ;
}
