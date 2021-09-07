#pragma once

#include "thread/thread.h"
#include "device/ata.h"

#include <cinttypes>

class Flasher : public Thread
{
private:
    Flasher (const Flasher &copy);
	char m_stack[16384];
protected:
    bool inquire(ATADevice& device);
    bool upload_code(ATADevice& device, const uint8_t* buf, unsigned int len);
    bool execute_code(ATADevice& device);
    void flash_sector(ATADevice& device, uint32_t sector_address, uint8_t* sector, uint32_t sector_size);
    void memdump(ATADevice& device, uint32_t address, uint32_t size);
public:
    Flasher() : Thread(&m_stack[16000]) {}
    void action();
};

extern Flasher app;
