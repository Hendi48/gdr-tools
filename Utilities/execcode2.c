/*
 * uploads any given MN103 code to
 * the Hitachi-LG Xbox360 drive and
 * causes the drive to execute it.
 *
 * author: Kevin East (SeventhSon)
 * email: kev@kev.nu
 * web: http://www.kev.nu/360/
 * date: 7th March 2006 (last updated: 4th April 2006)
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
	unsigned int i, len, reallen, blocks, blklen, sum;
	unsigned char param_list[0x10], *buffer;
	FILE *fptr;
	static unsigned char txblk[0x800];

	if(argc < 3) {
		printf(USAGE);
		return 1;
	}

	if((fd = open(argv[1], O_RDONLY | O_NONBLOCK)) == -1) {
		perror("open");
		return 1;
	}
	
	if(!(fptr = fopen(argv[2], "rb"))) {
		perror("fopen");
		close(fd);
		return 1;
	}

	if(fseek(fptr, 0, SEEK_END) == -1) {
		perror("fseek(SEEK_END)");
		close(fd);
		fclose(fptr);
		return 1;
	}
	if((len = ftell(fptr)) == -1) {
		perror("ftell");
		close(fd);
		fclose(fptr);
		return 1;
	}
	if(fseek(fptr, 0, SEEK_SET) == -1) {
		perror("fseek(SEEK_SET)");
		close(fd);
		fclose(fptr);
		return 1;
	}

	reallen = len;
	len += 2; // for checksum

	if(len & 0x03) // pad to nearest 4 byte boundary
		len += 4 - (len & 0x03);

	if(!(buffer = (unsigned char *)malloc(len))) {
		printf("malloc() failed\n");
		close(fd);
		fclose(fptr);
		return 1;
	}

	memset(buffer, 0, len);

	if(fread(buffer, reallen, 1, fptr) < 1)
		printf("fread() failed\n");
	else {
		// Calculate checksum
		for(sum = 0, i = 0; i < reallen; i++)
			sum += buffer[i];
		sum = 0x10000 - (sum & 0x0000FFFF);
		buffer[len - 2] = (unsigned char)(sum & 0x00FF);
		buffer[len - 1] = (unsigned char)((sum & 0xFF00) >> 8);				

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
			// Stop the disc
			memset(&cgc, 0, sizeof(cgc));
			cgc.cmd[0] = 0x1B;
	
			cgc.sense = &sense;
			cgc.data_direction = CGC_DATA_NONE;
			cgc.timeout = 15000;
			
			if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1 && (sense.sense_key != 0x02 || sense.asc != 0x3A)) {
				perror("ioctl");
				printf("failed to stop the disc (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
			}
			else {
				// <hack>
				// Clear (59E) bit 3 if it's set (this bit will cause us problems if it's left set)
				memset(&cgc, 0, sizeof(cgc));
				cgc.cmd[0] = 0x55;
		
				cgc.sense = &sense;
				cgc.data_direction = CGC_DATA_NONE;
				cgc.timeout = 15000;
				
				ioctl(fd, CDROM_SEND_PACKET, &cgc);
				// </hack>

				// Configure upload (required to upload/execute code)
				memset(&cgc, 0, sizeof(cgc));
				cgc.cmd[0] = 0x55;
				cgc.cmd[1] = 0x10;
				cgc.cmd[8] = 0x10;
				cgc.cmd[11] = 0x01;
						
				memset(param_list, 0, sizeof(param_list));
				param_list[9] = 0x06;
				param_list[10] = 0x48;
				param_list[11] = 0x4C;
				param_list[12] = (unsigned char)((len & 0xFF000000) >> 24);
				param_list[13] = (unsigned char)((len & 0x00FF0000) >> 16);
				param_list[14] = (unsigned char)((len & 0x0000FF00) >> 8);
				param_list[15] = (unsigned char)(len & 0x000000FF);
					
				cgc.buffer = param_list;
				cgc.buflen = 0x10;
				cgc.sense = &sense;
				cgc.data_direction = CGC_DATA_WRITE;
				cgc.timeout = 15000;
				
				if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
					perror("ioctl");
					printf("failed to configure upload (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
				}
				else {
					// Upload our buffer in ~2K blocks
					blocks = len / 0x7F8;
					if(len % 0x7F8)
						blocks++;
					for(sum = 0, i = 0; i < blocks; i++) {
						if(i != blocks - 1)
							blklen = 0x800;
						else
							blklen = (len % 0x7F8) + 8;
			
						memset(&cgc, 0, sizeof(cgc));
						cgc.cmd[0] = 0x55;
						cgc.cmd[1] = 0x10;
						cgc.cmd[7] = (unsigned char)((blklen & 0xFF00) >> 8);
						cgc.cmd[8] = (unsigned char)(blklen & 0x00FF);
						cgc.cmd[11] = 0x01;
			
						memset(txblk, 0, sizeof(txblk));
						txblk[4] = 0x48;
						txblk[5] = 0x4C;
	
						memcpy(&txblk[8], &buffer[i * 0x7F8], blklen - 8);
	
						cgc.buffer = txblk;
						cgc.buflen = blklen;
						cgc.sense = &sense;
						cgc.data_direction = CGC_DATA_WRITE;
						cgc.timeout = 15000;
			
						if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
							perror("ioctl");
							printf("failed to upload buffer block %u (sense: %02X/%02X/%02X)\n", i, sense.sense_key, sense.asc, sense.ascq);
							close(fd);
							fclose(fptr);
							free(buffer);
							return 1;
						}
					}
		
					// Execute code
					memset(&cgc, 0, sizeof(cgc));
					cgc.cmd[0] = 0x55;
					cgc.cmd[1] = 0x10;
					cgc.cmd[3] = 0x48;
					cgc.cmd[4] = 0x4C;
					cgc.cmd[6] = 0x01;
					cgc.cmd[11] = 0x01;
		
					cgc.sense = &sense;
					cgc.data_direction = CGC_DATA_NONE;
					cgc.timeout = 15000;
			
					if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
						perror("ioctl");
						printf("failed to execute flasher (sense: %02X/%02X/%02X)\n", sense.sense_key, sense.asc, sense.ascq);
					}
					else
						printf("done\n");
				}
			}
		}
	}

	close(fd);
	fclose(fptr);
	free(buffer);

	printf("\n");

	return 0;
}
