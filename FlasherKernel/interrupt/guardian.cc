#include "machine/plugbox.h"
#include "interrupt/guard.h"
#include "device/panic.h"
#include "machine/pic.h"

// Central interrupt handling routine
extern "C" void guardian (unsigned int slot, void *interrupt_context);

void guardian (unsigned int slot, void *interrupt_context)
{
	cur_interrupt_number = slot;
	cur_interrupt_context = interrupt_context;
	Gate& gate = plugbox.report(slot);
	if (gate.prologue()) {
		guard.relay(&gate);
	}

	if (slot >= 32 && slot <= 47) {
		pic.ack(slot - 32);
	}
}
