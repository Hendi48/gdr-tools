#include "interrupt/guard.h"

void Guard::leave() {
	Gate *gate;

	// Handle all queued epilogues
	do {
		cpu.disable_int(); // no interrupts while manipulating queue!
		gate = (Gate*) m_epilogues.dequeue();

		if (gate) {
			gate->queued(false);
			cpu.enable_int();
			gate->epilogue();
		}
	} while (gate);

	retne(); // make available
	cpu.enable_int();
}

void Guard::relay(Gate* item) {
	if (avail()) {
		// Epilogue level available, can immediately execute
		enter();
		cpu.enable_int();
		item->epilogue();
		leave();
	} else {
		// We're on the epilogue level, need to defer execution
		if (!item->queued()) {
			item->queued(true);
			m_epilogues.enqueue(item);
		}
	}
}

Guard guard;
