#include "machine/plugbox.h"
#include "device/panic.h"

Plugbox::Plugbox() {
	// Default: Map all to Panic
	for (int i = 0; i < IDT_COUNT; i++) {
		m_gates[i] = &panicDefaultGate;
	}
}

void Plugbox::assign(unsigned int slot, Gate& gate) {
	if (slot >= IDT_COUNT) {
		return;
	}
	m_gates[slot] = &gate;
}

Gate& Plugbox::report(unsigned int slot) {
	if (slot >= IDT_COUNT) {
		return panicDefaultGate;
	}
	return *m_gates[slot];
}

Plugbox plugbox;
