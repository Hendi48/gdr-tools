#pragma once

#include "machine/atactrl.h"
#include "machine/scsi.h"
#include "interrupt/gate.h"
#include "sync/condition.h"

class ATADevice : public Gate, public ATAController
{
private:
	ATADevice (const ATADevice &copy);

	int m_irq_line;
	Condition m_intrq_condition;

public:
    ATADevice(int command_block_base, int control_port, int irq_line)
		: ATAController(command_block_base, control_port),
		  m_irq_line(irq_line), m_intrq_condition() {};

	void plugin();

	bool prologue();
	void epilogue();

	bool read_data(uint8_t* buf, int words);
	bool request_sense(PSENSE_DATA sense);

	// Suspends thread until ATA interrupt arrives
	void wait_for_interrupt();
};
