#include "device/panic.h"
#include "device/cgastr.h"
#include "machine/cpu.h"

#include <cinttypes>

bool Panic::prologue() {
	kout << endl <<
	 "=========================" << endl <<
	 "PANIC!!!" << endl <<
	 "Interrupt: 0x" << hex << cur_interrupt_number << dec << endl <<
	 "Address: 0x" << hex << *(void**) (((char*) cur_interrupt_context) - 0x10) << dec << endl <<
	 "Address: 0x" << hex << *(void**) (((char*) cur_interrupt_context) - 0x8) << dec << endl <<
	 "Address: 0x" << hex << *(void**) (((char*) cur_interrupt_context)) << dec << endl <<
	 "=========================" << endl;
	kout.flush();
	cpu.halt();
	return false;
}

Panic panicDefaultGate;
int cur_interrupt_number;
void* cur_interrupt_context;
