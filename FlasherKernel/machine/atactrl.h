#pragma once

#include "machine/io_port.h"

#include <cinttypes>

// Status
#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Index
#define ATA_SR_ERR     0x01    // Error

// Errors
#define ATA_ER_BBK      0x80    // Bad block
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // Media changed
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

class ATAController
{
private:
    ATAController(const ATAController &copy);

protected:
    // Ports
    const IOPort data_port;
    const IOPort features_port; // READ: Error
    const IOPort sector_count_port;
    const IOPort sector_number_port;
    const IOPort byte_count_low_port;
    const IOPort byte_count_high_port;
    const IOPort drive_select_port;
    const IOPort command_port;
    const IOPort device_control_register_port; // READ: Alternate Status

    bool m_has_error;

    void acknowledge_interrupt();

public:

    enum Bus { Primary = 0x1F0, Secondary = 0x170 };
    enum Drive { Master = 0, Slave = 1 };

    enum ATAPICommand {
        Nop = 0x00,
        Packet = 0xA0,
        IdentifyPacketDevice = 0xA1,
    };

    ATAController (int command_block_base, int control_port);

    void toggle_ata_interrupts(bool enabled);

    void select_drive(Drive drive);
    void send_command(ATAPICommand command);
    void send_packet(unsigned char *packet, int transfer_byte_count);
    int read_buffer(unsigned char *buf, int words);
    int write_buffer(uint8_t *buf, int words);

    void software_reset();

    // Reads the drive register.
    int get_drive_register() { return drive_select_port.inb(); };
    // Reads the error register.
    int get_error_register() { return features_port.inb(); };
    // Reads the (alternate) status register.
    int get_status_register() { return device_control_register_port.inb(); };

    bool is_busy();

    void poll_busy();
    void poll_request_ready();

    bool has_error() { return m_has_error; };

    void dump_task_file();

protected:
    Bus m_bus;
};
