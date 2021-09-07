#include "machine/atactrl.h"
#include "machine/pic.h"

#include "device/cgastr.h"

#include <inttypes.h>

ATAController::ATAController (int command_block_base, int control_port) :
   data_port (command_block_base),
   features_port (command_block_base + 1),
   sector_count_port (command_block_base + 2),
   sector_number_port (command_block_base + 3),
   byte_count_low_port (command_block_base + 4),
   byte_count_high_port (command_block_base + 5),
   drive_select_port (command_block_base + 6),
   command_port (command_block_base + 7),
   device_control_register_port (control_port)
{

}

void ATAController::toggle_ata_interrupts(bool enabled)
{
    acknowledge_interrupt(); // Clear pending interrupt
    device_control_register_port.outb(enabled ? 0 : 2); // nIEN bit
}

void ATAController::acknowledge_interrupt()
{
    command_port.inb();
}

void ATAController::select_drive(Drive drive)
{
    drive_select_port.outb(0x20 | 0x40 | 0x80 | (drive << 4));
    // 4 Alternative Status queries for 400ns delay
    for (int i = 0; i < 4; i++)
        device_control_register_port.inb();
}

bool ATAController::is_busy()
{
    return device_control_register_port.inb() & ATA_SR_BSY;
}

void ATAController::poll_busy()
{
    while (device_control_register_port.inb() & ATA_SR_BSY)
        asm volatile ("pause");
}

void ATAController::poll_request_ready()
{
    // 4 Alternative Status queries for 400ns delay
    for (int i = 0; i < 4; i++)
        device_control_register_port.inb();

    while (device_control_register_port.inb() & ATA_SR_BSY)
        asm volatile ("pause");

    // "When BSY is cleared the other bits in this register shall be valid within 400ns"
    for (int i = 0; i < 4; i++)
        device_control_register_port.inb();

    int status = device_control_register_port.inb();
    if (status & ATA_SR_ERR) {
        m_has_error = true;
        return;
    }

    if (status & ATA_SR_DF) {
        kout << "DEVICE FAULT!" << endl;
        return;
    }

    if ((status & ATA_SR_DRQ) == 0) {
        kout << "NOT READY!" << endl;
        return;
    }
}

void ATAController::send_command(ATAPICommand command)
{
    m_has_error = false;

    features_port.outb(0);
    sector_count_port.outb(0);
    sector_number_port.outb(0);
    byte_count_low_port.outb(0);
    byte_count_high_port.outb(0);
    command_port.outb(command);

    for (int i = 0; i < 4; i++)
        device_control_register_port.inb();

    poll_busy();
}

void ATAController::send_packet(unsigned char *packet, int transfer_byte_count)
{
    m_has_error = false;

    features_port.outb(0);
    sector_count_port.outb(0);
    sector_number_port.outb(0);
    byte_count_low_port.outb(transfer_byte_count & 0xFF);
    byte_count_high_port.outb((transfer_byte_count >> 8) & 0xFF);
    command_port.outb(ATAPICommand::Packet);

    poll_request_ready();

    if (!m_has_error) {
        unsigned short* p = (unsigned short*) packet;
        for (int i = 0; i < 6; i++) {
            data_port.outw(*p++);
            //device_control_register_port.inb(); // wait one i/o cycle
        }
    } else {
        kout << "Didn't send packet contents" << endl;
    }
}

int ATAController::read_buffer(unsigned char *buf, int words)
{
    for (int i = 0; i < words; i++) {
        if (!(device_control_register_port.inb() & ATA_SR_DRQ)) {
            return i;
        }
        *((uint16_t*) buf) = data_port.inw();
        buf += 2;
    }
    return words;
}

int ATAController::write_buffer(uint8_t *buf, int words)
{
    for (int i = 0; i < words; i++) {
        if (!(device_control_register_port.inb() & ATA_SR_DRQ)) {
            return i;
        }
        data_port.outw(*(uint16_t*)buf);
        buf += 2;
    }
    return words;
}

void ATAController::software_reset()
{
    device_control_register_port.outb(4);

    for (int i = 0; i < 50; i++)
        device_control_register_port.inb();

    device_control_register_port.outb(0);

    for (int i = 0; i < 20000; i++)
        device_control_register_port.inb();

    poll_busy();
}

void ATAController::dump_task_file()
{
    for (int i = 0; i < 4; i++)
        device_control_register_port.inb();

    kout << hex <<
        "Status: " << device_control_register_port.inb() << "; " <<
        "Error: " << features_port.inb() << "; " <<
        "SecCount: " << sector_count_port.inb() << "; " <<
        "SecNum: " << sector_number_port.inb() << "; " <<
        "CountLo: " << byte_count_low_port.inb() << "; " <<
        "CountHi: " << byte_count_high_port.inb() << dec << endl;
    kout.flush();
}
