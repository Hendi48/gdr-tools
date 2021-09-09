/*
 * dumps the a given portion of the address space
 * of the MN103 microcontroller within the 
 * Hitachi-LG Xbox 360 DVD drive. Warning: it can
 * take a while to dump a lot of data.
 *
 * example: dump the internal memory mapped
 *          from 0x40000000 to 0x40020000
 *          with the 360 drive set up as /dev/hdc
 *          to the file image.bin using a block size
 *          of 0x8000.
 *
 * # ./memdump /dev/hdc 8000 4 8000 ./image.bin
 *
 * author: Kevin East (SeventhSon)
 * email: kev@kev.nu
 * web: http://www.kev.nu/360/
 * date: 12th Febuary 2006 (updated 15th March 2006)
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
#include <stdlib.h>


int hex_atoi(void *dest, char *src, int dest_len)
{
	int src_last, i, j;
	unsigned char val;
	unsigned short end;

	end = 0x00FF; // endian detection value

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

		if(*((unsigned char *)&end)) // little endian CPU
			j >>= 1;
		else // big endian CPU
			j = dest_len - ((j >> 1) + 1);

		if(j >= dest_len || j < 0)
			break;

		((unsigned char *)dest)[j] |= val;
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	int fd;
	unsigned int i, block_size, block_off, block_len;
	struct cdrom_generic_command cgc;
	struct request_sense sense;
	unsigned char *data;
	FILE *fptr;

	if(argc != 6) {
		printf("usage: memdump device offset_in_blocks length_in_blocks block_size output_file\n");
		return 1;
	}

	if(hex_atoi(&block_off, argv[2], sizeof(block_off))) {
		printf("invalid offet_in_blocks (hexadecimal digits only)\n");
		return 1;
	}
	if(hex_atoi(&block_len, argv[3], sizeof(block_len))) {
		printf("invalid length_in_blocks (hexadecimal digits only)\n");
		return 1;
	}
	if(hex_atoi(&block_size, argv[4], sizeof(block_size))) {
		printf("invalid block_size (hexadecimal digits only)\n");
		return 1;
	}
	if(!block_size || block_size > 65535) {
		printf("invalid block_size (valid: 1 - 65535)\n");
		return 1;
	}

	if(!(data = malloc(block_size))) {
		printf("malloc() failed\n");
		return 1;
	}

	if((fd = open(argv[1], O_RDONLY | O_NONBLOCK)) == -1) {
		free(data);
		perror("open");
		return 1;
	}

	if(!(fptr = fopen(argv[5], "wb"))) {
		free(data);
		close(fd);
		perror("fopen");
		return 1;
	}
	
	for(i = block_off; i < block_off + block_len; i++) {
		memset(data, '0', sizeof(data));
		memset(cgc.cmd, 0, sizeof(cgc.cmd));
		cgc.cmd[0] = 0xE7; // vendor specific command (discovered by DaveX)
		cgc.cmd[1] = 0x48; // H
		cgc.cmd[2] = 0x49; // I
		cgc.cmd[3] = 0x54; // T
		cgc.cmd[4] = 0x01; // read MCU memory sub-command
		cgc.cmd[6] = (unsigned char)(((i * block_size) & 0xFF000000) >> 24); // address MSB
		cgc.cmd[7] = (unsigned char)(((i * block_size) & 0x00FF0000) >> 16); // address
		cgc.cmd[8] = (unsigned char)(((i * block_size) & 0x0000FF00) >> 8); // address
		cgc.cmd[9] = (unsigned char)((i * block_size) & 0x000000FF); // address LSB
		cgc.cmd[10] = (unsigned char)((block_size & 0xFF00) >> 8); // length MSB
		cgc.cmd[11] = (unsigned char)(block_size & 0x00FF); // length LSB

		cgc.buffer = data;
		cgc.buflen = block_size;
		cgc.sense = &sense;
		cgc.data_direction = CGC_DATA_READ;
		cgc.timeout = 15000;

		if(ioctl(fd, CDROM_SEND_PACKET, &cgc) == -1) {
			perror("ioctl");
			printf("sense: %02X/%02X/%02X (offset 0x%08X)\n", sense.sense_key, sense.asc, sense.ascq, i * block_size);
		}
		else {
			fwrite(data, block_size, 1, fptr);
			if(ferror(fptr))
				printf("fwrite() failed (offset 0x%08X)\n", i * block_size);
		}
	}
	
	printf("\n");

	free(data);
	fclose(fptr);
	close(fd);

	return 0;
}
