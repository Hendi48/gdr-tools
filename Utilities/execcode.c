/*
 * uploads any given MN103 code to
 * the Hitachi-LG Xbox360 drive and
 * causes the drive to execute it.
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

#define USAGE "usage: execcode device binary_code_file\n\n"

int main(int argc, char *argv[])
{
	int fd;
	struct cdrom_generic_command cgc;
	struct request_sense sense;
	unsigned int i, len;
	unsigned char param_list[8];
	FILE *fptr;

	if(argc < 3) {
		printf(USAGE);
		return 1;
	}

	if((fd = open(argv[1], O_RDONLY | O_NONBLOCK)) == -1) {
		perror("open");
		return 1;
	}

	if(!(fptr = fopen(argv[2], "rb"))) {
		close(fd);
		perror("fopen");
		return 1;
	}

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
		if(fseek(fptr, 0, SEEK_END) == -1) {
			close(fd);
			fclose(fptr);
			perror("fseek");
			return 1;
		}
		if((len = ftell(fptr)) == -1) {
			close(fd);
			fclose(fptr);
			perror("ftell");
			return 1;
		}
		if(fseek(fptr, 0, SEEK_SET) == -1) {
			close(fd);
			fclose(fptr);
			perror("fseek");
			return 1;
		}
		// upload code a byte at a time using RAM poke command
		for(i = 0; i < len; i++) {
			// Hitachi poke RAM command
			memset(cgc.cmd, 0, sizeof(cgc.cmd));
			cgc.cmd[0] = 0xE7;
			cgc.cmd[1] = 0x48;
			cgc.cmd[2] = 0x49;
			cgc.cmd[3] = 0x54;
			cgc.cmd[4] = 0xCC;
			cgc.cmd[5] = (unsigned char)getc(fptr);
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
				fclose(fptr);
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

	close(fd);
	fclose(fptr);

	printf("\n");

	return 0;
}
