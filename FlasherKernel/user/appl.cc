#include "user/appl.h"
#include "device/ata.h"
#include "device/cgastr.h"
#include "machine/atactrl.h"
#include "machine/key.h"
#include "machine/pic.h"
#include "machine/cpu.h"
#include "machine/pci.h"
#include "machine/scsi.h"
#include "interrupt/secure.h"

#include "syscall/guarded_keyboard.h"
#include "syscall/guarded_scheduler.h"

#include "user/codeblobs.h"
#include "user/xbox.h"

// ################# Configuration #################

#define PCI_BUS 3
#define PCI_SLOT 0
#define PCI_FUNC 0

// #################################################

int minX, minY;

void backspace()
{
	int x, y;
	kout.getpos(x, y);
	if (x && (y > minY || (y == minY && x > minX))) {
		x--;
		kout.show(x, y, 0, 7);
		kout.setpos(x, y);
	}
}

void read_input(const char* prompt, char* buf, unsigned int buf_size)
{
	kout << prompt;
	kout.flush();
	kout.getpos(minX, minY);

	unsigned int i = 0;
	while (1) {
		Key k = keyboard.getkey();
		if ((char) k == '\n') {
			kout << endl;
			break;
		}
		if ((char) k == 8) {
			if (i > 0) {
				i--;
				backspace();
			}
			continue;
		}
		if (i < buf_size - 1) {
			buf[i++] = (char) k;
			kout << (char) k;
			kout.flush();
		}
	}
	buf[i] = 0;
}

// Copies up to n hex chars from input to output. Output must have space for n+1.
// Returns input pointer after last hex char, or nullptr if this was no hex token.
char* hex_tok(char* input, char* output, int n)
{
	int i = 0;
	while (*input && i < n) {
		char c = *input;
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
			input++;
			output[i++] = c;
		} else {
			break;
		}
	}
	output[i] = 0;
	return i > 0 ? input : nullptr;
}

uint32_t hex_to_int(char* hex)
{
	uint32_t result = 0;
	while (*hex) {
    	char here = *hex++;
		if (here >= 'A')
	  		here |= 0x20;
      	int digit = (here <= '9') ? (here - '0') : (10 + (here - 'a'));
		result <<= 4;
		result |= digit;
    }
	return result;
}

void Flasher::action()
{
	//kout << __builtin_extract_return_addr(__builtin_return_address(0)) << endl;
	kout << "== LG GDR Flasher ==" << endl;
	kout.flush();

	int pci_ctrl;
	while ((pci_ctrl = pci.config_read_dword(PCI_BUS, PCI_SLOT, PCI_FUNC, 0)) == -1) {
		kout << "IDE controller not found on bus" << endl;
		kout.flush();
		keyboard.getkey();
	}
	kout << "Controller: " << hex << pci_ctrl << dec << endl;
	kout << "Class: " << hex << pci.config_read_byte(PCI_BUS, PCI_SLOT, PCI_FUNC, 0xB) << dec << endl;
	kout << "ProgIf: " << hex << pci.config_read_byte(PCI_BUS, PCI_SLOT, PCI_FUNC, 0x9) << dec << endl;
	kout.flush();

	int bar0 = pci.config_read_dword(PCI_BUS, PCI_SLOT, PCI_FUNC, 0x10) & 0xFFFFFFFC;
	int bar1 = pci.config_read_dword(PCI_BUS, PCI_SLOT, PCI_FUNC, 0x14) & 0xFFFFFFFC;
	int bar2 = pci.config_read_dword(PCI_BUS, PCI_SLOT, PCI_FUNC, 0x18) & 0xFFFFFFFC;
	int bar3 = pci.config_read_dword(PCI_BUS, PCI_SLOT, PCI_FUNC, 0x1C) & 0xFFFFFFFC;
	int line = pci.config_read_byte(PCI_BUS, PCI_SLOT, PCI_FUNC, 0x3C);
	kout << "BAR0: " << hex << bar0 << dec << endl;
	kout << "BAR1: " << hex << bar1 << dec << endl;
	kout << "BAR2: " << hex << bar2 << dec << endl;
	kout << "BAR3: " << hex << bar3 << dec << endl;
	kout << "Line: " << line << endl;
	kout << "Pin : " << pci.config_read_byte(PCI_BUS, PCI_SLOT, PCI_FUNC, 0x3D) << endl;
	kout.flush();

	ATADevice ide(bar0, bar1 + 2, line);
	ide.plugin(); // setup interrupt handling

	DRIVE_LOOP:
	for (;;) {
		kout << "Select drive (0 = Master, 1 = Slave, r = Software Reset): ";
		kout.flush();

		int drive = -1;
		do {
			Key k = keyboard.getkey();
			if ((char) k == 'r') {
				kout << k << endl;
				kout << "Doing software reset... ";
				kout.flush();
				ide.software_reset();
				kout << "OK" << endl;
				goto DRIVE_LOOP;
			} else if ((char) k == '0' || (char) k == '1') {
				drive = ((char) k) == '0' ? 0 : 1;
				kout << k;
			}
		} while (drive == -1);
		kout << endl;

		ide.toggle_ata_interrupts(true); // this is required for proper operation

		kout << "Selecting drive..." << endl;
		ide.select_drive((ATAController::Drive) drive);
		int drive_reg = ide.get_drive_register();
		kout << "Drive Register: " << hex << drive_reg << dec << endl;
		kout.flush();

		ide.toggle_ata_interrupts(true);

		if (inquire(ide)) {
			while (1) {
				char command[32];
				read_input("Flasher>", command, 32);
				if ((!command[1] || !command[2])
						&& command[0] != 'i'
						&& command[0] != 'a'
						&& command[0] != 'n'
						&& command[0] != 'c') {
					kout << "Incomplete input" << endl;
					continue;
				}

				unsigned char* code;
				unsigned int code_size;
				switch (command[0]) {
					// ===== RECOVERY MODE =====
					// Identify flash chip
					case 'i': {
						code = id_code;
						code_size = sizeof(id_code);
						break;
					}
					// Read 4 bytes
					case 'r': {
						char addr_buf[9];
						char* remainder = hex_tok(&command[2], addr_buf, 8);
						if (!remainder || *remainder) {
							kout << "Malformed input (r <hex addr>)" << endl;
							continue;
						}
						uint32_t conv = hex_to_int(addr_buf);
						*(uint32_t*) &read_code[0x5] = conv;
						*(uint32_t*) &read_code[0xE] = conv + 1;
						*(uint32_t*) &read_code[0x17] = conv + 2;
						*(uint32_t*) &read_code[0x20] = conv + 3;
						code = read_code;
						code_size = sizeof(read_code);
						break;
					}
					// Write byte
					case 'w': {
						char addr_buf[9];
						char* remainder = hex_tok(&command[2], addr_buf, 8);
						if (!remainder || *remainder != ' ') {
							kout << "Malformed input (w <hex addr> <byte>)" << endl;
							continue;
						}
						char byte_buf[3];
						remainder = hex_tok(remainder + 1, byte_buf, 2);
						if (!remainder || *remainder) {
							kout << "Malformed input (w <hex addr> <byte>)" << endl;
							continue;
						}

						uint32_t addr = hex_to_int(addr_buf);
						uint8_t data = hex_to_int(byte_buf);

						*(uint32_t*) &write_code[0x13] = addr;
						*(uint8_t*) &write_code[0x18] = data;
						code = write_code;
						code_size = sizeof(write_code);
						break;
					}
					// Erase sector
					case 'e': {
						char addr_buf[9];
						char* remainder = hex_tok(&command[2], addr_buf, 8);
						if (!remainder || *remainder != ' ') {
							kout << "Malformed input (e <hex addr> <size>)" << endl;
							continue;
						}
						char size_buf[6];
						remainder = hex_tok(remainder + 1, size_buf, 5);
						if (!remainder || *remainder) {
							kout << "Malformed input (e <hex addr> <size>)" << endl;
							continue;
						}

						uint32_t addr = hex_to_int(addr_buf);
						uint32_t size = hex_to_int(size_buf);

						*(uint32_t*) &erase_code[0x13] = addr;
						*(uint32_t*) &erase_code[0x19] = size;
						code = erase_code;
						code_size = sizeof(erase_code);
						break;
					}
					// Flash a predefined sector
					case 'f': {
						char addr_buf[9];
						char* remainder = hex_tok(&command[2], addr_buf, 8);
						if (!remainder || *remainder) {
							kout << "Malformed input (f <hex addr>)" << endl;
							continue;
						}

						uint32_t addr = hex_to_int(addr_buf);
						flash_sector(ide, addr, sector_to_flash, sizeof(sector_to_flash));
						continue;
					}
					// ===== END RECOVERY MODE =====
					// Read memory using command
					case 'm': {
						char addr_buf[9];
						char* remainder = hex_tok(&command[2], addr_buf, 8);
						if (!remainder || *remainder != ' ') {
							kout << "Malformed input (m <hex addr> <size>)" << endl;
							continue;
						}
						char size_buf[6];
						remainder = hex_tok(remainder + 1, size_buf, 5);
						if (!remainder || *remainder) {
							kout << "Malformed input (m <hex addr> <size>)" << endl;
							continue;
						}
						uint32_t addr = hex_to_int(addr_buf);
						uint32_t size = hex_to_int(size_buf);

						memdump(ide, addr, size);
						continue;
					}
					// Read disc capacity
					case 'c': {
						uint8_t command[12] = {SCSI_READ_CAPACITY, 0};
						ide.send_packet(command, sizeof(READ_CAPACITY_DATA));
						ide.poll_request_ready();
						if (!ide.has_error()) {
							READ_CAPACITY_DATA capacity;
							ide.read_buffer((uint8_t*) &capacity, sizeof(READ_CAPACITY_DATA) / 2);
							capacity.LogicalBlockAddress = __builtin_bswap32(capacity.LogicalBlockAddress);
							capacity.BytesPerBlock = __builtin_bswap32(capacity.BytesPerBlock);
							kout << "LBA: " << capacity.LogicalBlockAddress << "; BytesPerBlock: " << capacity.BytesPerBlock << " ==> " << (uint64_t)(capacity.LogicalBlockAddress + 1) * capacity.BytesPerBlock << " bytes" << endl;
							kout.flush();
						}
						continue;
					}
					// Read sector
					case 's': {
						char lba_buf[9];
						char* remainder = hex_tok(&command[2], lba_buf, 8);
						if (!remainder || *remainder != ' ') {
							kout << "Malformed input (s <lba> <size>)" << endl;
							continue;
						}
						char size_buf[5];
						remainder = hex_tok(remainder + 1, size_buf, 4);
						if (!remainder || *remainder) {
							kout << "Malformed input (s <lba> <size>)" << endl;
							continue;
						}
						uint32_t lba = hex_to_int(lba_buf);
						uint16_t size = hex_to_int(size_buf);

						uint8_t command[12] = {SCSI_READ, 0, (uint8_t)(lba >> 24), (uint8_t)(lba >> 16), (uint8_t)(lba >> 8), (uint8_t)lba, 0, 2048 >> 8, 2048 & 0xFF, 0};
						ide.send_packet(command, 2048);
						ide.poll_request_ready();
						if (!ide.has_error()) {
							uint8_t buf[2048];
							ide.read_buffer(buf, 2048 / 2);
							for (int i = 0; i < size; i++) {
								kout << hex << (int) buf[i] << " " << dec;
							}
							kout << endl;
							kout.flush();
						}
						continue;
					}
					// ===== XBOX SPECIFIC =====
					// Authenticate an XGD via challenge/response
					case 'a': {
						XboxAuth xbox(ide);
						xbox.authenticate_disc();
						continue;
					}
					// This is a "cheat" for the 8050L FW, it'll cause it to immediately set itself to authorized and switch to the Xbox partition -- oopsie Hitachi devs?
					case 'n': {
						ide.send_command(ATAController::ATAPICommand::Nop);
						continue;
					}
					default:
						kout << "Unknown command" << endl;
						continue;
				}

				if (upload_code(ide, code, code_size)) {
					kout << "Uploaded, executing..." << endl;
					execute_code(ide);
				}
			}
		}
		kout << endl;
	}
}

#define SCSI_HITACHI_BACKDOOR 0xE7

bool Flasher::inquire(ATADevice& device)
{
	unsigned char inquiry[12] = {SCSI_INQUIRY, 0, 0, 0, 96, 0};
	device.send_packet(inquiry, 96);
	//device.wait_for_interrupt();
	device.poll_request_ready();
	if (device.has_error()) {
		kout << "Error: " << hex << device.get_error_register() << dec << endl;
		kout.flush();
	}

	bool result = !device.has_error();
	if (result) {
		unsigned char buf[96];
		int words_read = device.read_buffer(buf, 48);
		// for (int i = 0; i < 96; i++) {
		// 	kout << hex << (int) buf[i] << dec << " ";
		// }
		// kout << endl;
		if (words_read < 48) {
			kout << "Only " << words_read << " words have been read" << endl;
			result = false;
		} else {
			if (buf[0] != 5) {
				kout << "Type is not disc drive, got " << (int) buf[0] << endl;
				result = false;
			} else {
				char drive_ident[49];
				for (int i = 0; i < 48; i++) {
					drive_ident[i] = buf[8 + i];
				}
				drive_ident[48] = 0;
				kout << "Identified drive: " << drive_ident << endl;
			}
		}

		kout.flush();
	}

	//device.poll_busy();

	//device.dump_task_file();

	return result;
}

bool Flasher::upload_code(ATADevice& device, const uint8_t* buf, unsigned int len)
{
	if (len > 4798) {
		kout << "Buf exceeds stack array" << endl;
		kout.flush();
		return false;
	}
	uint8_t device_buf[4800];
	unsigned int i, sum = 0;

	for (i = 0; i < len; i++) {
		device_buf[i] = buf[i];
	}
	if (len & 1) {
		device_buf[len++] = 0;
	}

	for (i = 0; i < len; i++) {
		sum += device_buf[i];
	}
	sum = 0x10000 - (sum & 0xFFFF);
	device_buf[len++] = sum & 0xFF;
	device_buf[len++] = (sum >> 8) & 0xFF;

	uint8_t command[12] = {SCSI_HITACHI_BACKDOOR, 0, 0, 0, 0x3B, 0x6, 0, 0, 0, 0, (uint8_t) (len >> 8), (uint8_t) (len & 0xFF)};
	device.send_packet(command, len);
	{
		Secure sec;
		device.wait_for_interrupt();
	}
	device.poll_request_ready();

	bool result = !device.has_error();
	if (result) {
		int words = len / 2;
		int written = device.write_buffer(device_buf, words);
		if (written < words) {
			kout << "Incomplete, wrote " << written << endl;
			kout.flush();
			return false;
		}

		{
			Secure sec;
			device.wait_for_interrupt();
		}
		if (device.has_error()) {
			kout << "Error: " << hex << device.get_error_register() << dec << endl;
			kout.flush();
			result = false;
		}
	} else {
		kout << "Error: " << hex << device.get_error_register() << dec << endl;
		kout.flush();
	}

	return result;
}

bool Flasher::execute_code(ATADevice& device)
{
	uint8_t command[12] = {SCSI_HITACHI_BACKDOOR, 0, 0, 0, 0x3B, 0x7, 0, 0, 0, 0, 0, 0};
	device.send_packet(command, 0);
	{
		Secure sec;
		device.wait_for_interrupt();
	}
	return true;
}

void Flasher::flash_sector(ATADevice& device, uint32_t sector_address, uint8_t* sector, uint32_t sector_size)
{
	// Transmit in chunks of 0x1000
	kout << "Flashing..." << endl;
	for (uint32_t done = 0; done < sector_size; done += 0x1000) {
		uint8_t code_appended[sizeof(write_sector) + 0x1000];
		uint32_t chunk_size = (sector_size - done >= 0x1000 ? 0x1000 : sector_size - done);
		*(uint32_t*) &write_sector[0x13] = sector_address;
		*(uint32_t*) &write_sector[0x19] = chunk_size;
		sector_address += chunk_size;

		unsigned int i;
		for (i = 0; i < sizeof(write_sector); i++) {
			code_appended[i] = write_sector[i];
		}
		for (unsigned int i_start = i; i < i_start + chunk_size; i++) {
			code_appended[i] = sector_to_flash[done + i - i_start];
		}

		unsigned int code_size = sizeof(write_sector) + chunk_size;
		if (upload_code(device, code_appended, code_size)) {
			kout << "0x" << hex << done << dec << endl;
			kout.flush();
			execute_code(device);
			kout << "OK" << endl;
			kout.flush();
			//for (int d = 0; d < 1000000; d++)
			//	ide.get_status_register();
		}
	}
}

void Flasher::memdump(ATADevice& device, uint32_t address, uint32_t size)
{
	if (size > 0x1000)	{
		kout << "Only sizes up to 0x1000 are supported" << endl;
		return;
	}
	uint16_t device_size = size;
	if (device_size & 1) {
		device_size++;
	}

	uint8_t command[12] = {SCSI_HITACHI_BACKDOOR, 'H', 'I', 'T', 0x1, 0,
		(uint8_t)(address >> 24), (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address,
		(uint8_t)(device_size >> 8), (uint8_t)device_size};
	device.send_packet(command, device_size);
	{
		Secure sec;
		device.wait_for_interrupt();
	}
	device.poll_request_ready();

	uint8_t buf[0x1000];
	int read = device.read_buffer(buf, device_size / 2);
	if (read < device_size / 2) {
		kout << "[!] Only read " << read << endl;
	}

	for (unsigned i = 0; i < size; i++) {
		kout << hex << (int) buf[i] << " " << dec;
	}
	kout << endl;
}
