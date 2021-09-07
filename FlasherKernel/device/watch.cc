#include "device/watch.h"
#include "machine/plugbox.h"
#include "machine/pic.h"
#include "syscall/guarded_scheduler.h"

void Watch::plugin() {
	plugbox.assign(Plugbox::timer, *this);
	pic.allow(PIC::timer);
}

bool Watch::prologue() {
	return true;
}

void Watch::epilogue() {
	scheduler.resume();
}
