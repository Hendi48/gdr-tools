/*
 * Puts a Hitachi-LG Xbox 360 DVD drive into modeB.
 * In modeB, the drive responds to standard ATAPI
 * commands that it otherwise wouldn't (for example:
 * Read(12), Inquiry, Mode Sense(10)). ModeB also
 * changes, among other things, the behaviour of the
 * drive's eject input and tray_status output.
 *
 * author: Kevin East (SeventhSon)
 * email: kev@kev.nu
 * web: http://www.kev.nu/360/
 * date: 2nd March 2006
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

int main(int argc, char *argv[])
{
	int fd;
	struct cdrom_generic_command cgc;
	struct request_sense sense;

	if(argc != 2) {
		printf("usage: modeb device\n");
		return 1;
	}

	if((fd = open(argv[1], O_RDONLY | O_NONBLOCK)) == -1) {
		perror("open");
		return 1;
	}

	memset(&cgc, 0, sizeof(cgc));
	cgc.cmd[0] = 0xE7; // Hitachi debug command 0xE7
	cgc.cmd[1] = 0x48; // 'H'
	cgc.cmd[2] = 0x49; // 'I'
	cgc.cmd[3] = 0x54; // 'T'
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
		printf("sense: %02X/%02X/%02X\n", sense.sense_key, sense.asc, sense.ascq);
	}
	else
		printf("done\n");

	printf("\n");

	close(fd);

	return 0;
}
