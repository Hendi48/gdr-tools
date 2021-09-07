#include "device/ata.h"
#include "device/cgastr.h"
#include "machine/plugbox.h"
#include "machine/pic.h"
#include "machine/cpu.h"

#include "interrupt/secure.h"

void ATADevice::plugin()
{
	plugbox.assign(m_irq_line + 32, *this);
	pic.allow(m_irq_line);
}

bool ATADevice::prologue()
{
	// Read status port (required by spec)
	acknowledge_interrupt();

	toggle_ata_interrupts(false);

	m_has_error = get_status_register() & ATA_SR_ERR;

    kout << "! ";
	dump_task_file();

	toggle_ata_interrupts(true);

	return true;
}

void ATADevice::epilogue()
{
	m_intrq_condition.signal();
}

void ATADevice::wait_for_interrupt()
{
	m_intrq_condition.wait();
}

bool ATADevice::read_data(uint8_t* buf, int words)
{
    int read = 0;
    while (true) {
        read += read_buffer(buf + read * 2, words - read);
		if (read < words) {
			poll_request_ready();
			if (has_error() || !(get_status_register() & ATA_SR_DRQ)) {
				return false; // if DRQ wasn't set, the device forgot its command and we need to issue it again
			}
		} else {
			return true;
		}
    }
}

bool ATADevice::request_sense(PSENSE_DATA sense)
{
	uint8_t command[12] = {SCSI_REQUEST_SENSE, 0, 0, 0, sizeof(SENSE_DATA), 0};
	send_packet(command, sizeof(SENSE_DATA));
	{
		Secure sec;
		wait_for_interrupt();
	}

	poll_request_ready();
	if (has_error()) {
		return false;
	}

	return read_buffer((uint8_t*)sense, sizeof(SENSE_DATA) / 2) == sizeof(SENSE_DATA) / 2;
}
