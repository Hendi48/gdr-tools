/*
 * Reads or writes a single byte from/to
 * anywhere within the address space of
 * the MN103 microcontroller inside of
 * the Hitachi-LG Xbox360 drive.
 *
 * author: Kevin East (SeventhSon)
 * email: kev@kev.nu
 * web: http://www.kev.nu/360/
 * date: 7th March 2006
 * platform: linux
 *
 */

#include <stdio.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define USAGE "usage: pp device address_in_hex peek\n       pp device address_in_hex poke value_in_hex\n\n"
#define MODE_PEEK 0
#define MODE_POKE 1

/*
mov 0x000000??,D1
movbu D1,(0x????????)
rets
*/
unsigned char code[] = {0xFC,0xCD,0x00,0x00,0x00,0x00,0xFC,0x86,0x00,0x00,0x00,0x00,0xF0,0xFC};

int hex_atoi(void *dest, char *src, int dest_len)
{
	int src_last, i, j;
	unsigned char val;

	memset(dest, 0, dest_len);

	src_last = strlen(src) - 1;

	for(i = src_last; i >= 0; i--) {
		if(src[i] >= '0' && src[i] <= '9')
			val = src[i] - 0x30;
		else if(src[i] >= 'a' && src[i] <= 'f')
			val = (src[i] - 0x60) + 9;
		else if(src[i] >= 'A' && src[i] <= 'F')
			val = (src[i] - 0x40) + 9;
		else
			return 1; // invalid hex digit

		j = src_last - i;

		if(j & 1)
			val <<= 4;

		j >>= 1;

		if(j >= dest_len || j < 0)
			break;

		((unsigned char *)dest)[j] |= val;
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	int fd;
	struct cdrom_generic_command cgc;
	struct request_sense sense;
	unsigned int addr, mode, i;
	unsigned char val, param_list[8];

	if(argc < 4) {
		printf(USAGE);
		return 1;
	}

	if(hex_atoi(&addr, argv[2], sizeof(addr))) {
		printf("error: invalid address %s\n", argv[2]);
		printf(USAGE);
		return 1;
	}

	if(!strcmp(argv[3], "peek")) {
		mode = MODE_PEEK;
		val = 0;
	}
	else if(!strcmp(argv[3], "poke")) {
		if(argc < 5) {
			printf(USAGE);
			return 1;
		}
		mode = MODE_POKE;
		if(hex_atoi(&val, argv[4], sizeof(val))) {
			printf("error: invalid poke value %s\n", argv[4]);
			printf(USAGE);
			return 1;
		}
	}
	else {
		printf("error: invalid mode %s\n", argv[3]);
		printf(USAGE);
		return 1;
	}

	if((fd = open(argv[1], O_RDONLY | O_NONBLOCK)) == -1) {
		perror("open");
		return 1;
	}

	if(mode == MODE_PEEK) {
		// Hitachi read memory command
		memset(cgc.cmd, 0, sizeof(cgc.cmd));
		cgc.cmd[0] = 0xE7;
		cgc.cmd[1] = 0x48;
		cgc.cmd[2] = 0x49;
		cgc.cmd[3] = 0x54;
		cgc.cmd[4] = 0x01;
		cgc.cmd[6] = (unsigned char)((addr & 0xFF000000) >> 24); // address MSB
		cgc.cmd[7] = (unsigned char)((addr & 0x00FF0000) >> 16); // address
		cgc.cmd[8] = (unsigned char)((addr & 0x0000FF00) >> 8); // address
		cgc.cmd[9] = (unsigned char)(addr & 0x000000FF); // address LSB
		cgc.cmd[11] = 1; // length LSB

		cgc.buffer = &val;
		cgc.buflen = 1;
		cgc.sense = &sense;
		cgc.data_direction = CGC_DATA_READ;
		cgc.timeout = 15000;

		if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
			perror("ioctl");
			printf("Hitachi read memory command failed (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
		}
		else
			printf("0x%02X\n", val);
	}
	else { // mode == MODE_POKE
		// initiate modeB
		memset(&cgc, 0, sizeof(cgc));
		cgc.cmd[0] = 0xE7;
		cgc.cmd[1] = 0x48;
		cgc.cmd[2] = 0x49;
		cgc.cmd[3] = 0x54;
		cgc.cmd[4] = 0x30;
		cgc.cmd[5] = 0x90;
		cgc.cmd[6] = 0x90;
		cgc.cmd[7] = 0xD0;
		cgc.cmd[8] = 0x01;
		cgc.sense = &sense;
		cgc.data_direction = CGC_DATA_NONE;
		cgc.timeout = 15000;
		if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
			perror("ioctl");
			printf("failed to initiate modeB (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
		}
		else {
			*((unsigned int *)&code[8]) = addr;
			*((unsigned int *)&code[2]) = val;

			// upload code a byte at a time using RAM poke command
			for(i = 0; i < sizeof(code); i++) {
				// Hitachi poke RAM command
				memset(cgc.cmd, 0, sizeof(cgc.cmd));
				cgc.cmd[0] = 0xE7;
				cgc.cmd[1] = 0x48;
				cgc.cmd[2] = 0x49;
				cgc.cmd[3] = 0x54;
				cgc.cmd[4] = 0xCC;
				cgc.cmd[5] = code[i];
				cgc.cmd[8] = (unsigned char)(((0x80000000 + i) & 0xFF000000) >> 24); // address MSB
				cgc.cmd[9] = (unsigned char)(((0x80000000 + i) & 0x00FF0000) >> 16); // address
				cgc.cmd[10] = (unsigned char)(((0x80000000 + i) & 0x0000FF00) >> 8); // address
				cgc.cmd[11] = (unsigned char)((0x80000000 + i) & 0x000000FF); // address LSB
				cgc.sense = &sense;
				cgc.data_direction = CGC_DATA_NONE;
				cgc.timeout = 15000;
				if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
					perror("ioctl");
					printf("Hitachi poke RAM command failed on byte %u (sense: %02X/%02X/%02X)\n", i, sense.sense_key, sense.asc, sense.ascq);
					close(fd);
					return 1;
				}
			}

			// set (59E) bit 3 via Mode Select(10)
			memset(cgc.cmd, 0, sizeof(cgc.cmd));
			cgc.cmd[0] = 0x55;
			cgc.cmd[1] = 0x10;
			cgc.cmd[8] = 0x08;
			memset(param_list, 0, sizeof(param_list));
			param_list[1] = 6;
			cgc.buffer = param_list;
			cgc.buflen = 8;
			cgc.sense = &sense;
			cgc.data_direction = CGC_DATA_WRITE;
			cgc.timeout = 15000;
			if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
				perror("ioctl");
				printf("set (59E) bit 3 via Mode Select(10) failed (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
			}
			else {
				// jump to RAM routine via Mode Select(10)
				memset(cgc.cmd, 0, sizeof(cgc.cmd));
				cgc.cmd[0] = 0x55;
				cgc.cmd[1] = 0x10;
				cgc.cmd[3] = 0x48; // 'H'
				cgc.cmd[4] = 0x4C; // 'L'
				cgc.cmd[8] = 0x08;
				memset(param_list, 0, sizeof(param_list));
				param_list[1] = 6;
				cgc.buffer = param_list;
				cgc.buflen = 8;
				cgc.sense = &sense;
				cgc.data_direction = CGC_DATA_WRITE;
				cgc.timeout = 15000;
				if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1)
					printf("done\n");
				else
					printf("this shouldn't happen\n");
			}
		}
	}

	close(fd);

	printf("\n");

	return 0;
}
