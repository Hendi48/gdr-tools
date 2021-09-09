/**
 * Patches bytes at a given offset in an encrypted firmware,
 * by decrypting, patching and re-encrypting. Only works with "raw"
 * 256K firmwares, not DLDs.
 * 
 * Author: Hendi (code based on FirmCrypt by loser)
 * Platform: Any
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

uint32_t swap_bits(uint32_t chunk, const int* bits)
{
    uint32_t result = 0;
    
    int i;
    for (i = 0; i < 32; i++) {
        uint32_t bit = (chunk & (1 << bits[i])) >> bits[i];
        result = (result << 1) | bit;
    }
    
    return result;

}

const int ENCRYPT_BITS[32] = { 22, 27,  3, 10, 14, 16,  1, 28,
                               26,  7, 15, 21,  5, 19, 29, 12,
                                2, 11, 24, 20, 13, 18,  4, 30,
                                9, 17, 23,  0,  6, 31,  8, 25 };

void encrypt_firmware(uint8_t* input_buffer, uint8_t* output_buffer, int size)
{
    // process each 32bit chunk
    for (int i = 0; i < size; i += 4) {
        // get 'decrypted' chunk
        uint32_t chunk = (input_buffer[i+0]<<24) |
                    (input_buffer[i+1]<<16) |
                    (input_buffer[i+2]<< 8) |
                    (input_buffer[i+3]<< 0);
        
        // encrypt chunk
        chunk ^= 0x8b8b8b8b;
        chunk = swap_bits(chunk, ENCRYPT_BITS);
        
        // output 'encrypted' chunk
        output_buffer[i+0] = (chunk>>24);
        output_buffer[i+1] = (chunk>>16);
        output_buffer[i+2] = (chunk>> 8);
        output_buffer[i+3] = (chunk>> 0);
    }
}

const int DECRYPT_BITS[32] = {  2,  8, 17, 24, 30, 23,  0, 13,
                                5, 31, 20, 12, 18, 10,  6, 26,
                               21, 27, 11, 16, 14, 28,  7,  1,
                               22,  3, 19,  9, 29, 15, 25,  4 };

void decrypt_firmware(uint8_t* input_buffer, uint8_t* output_buffer, int size)
{
    // process each 32bit chunk
    for (int i = 0; i < size; i += 4) {
        // get 'encrypted' chunk
        uint32_t chunk = (input_buffer[i+0]<<24) |
                    (input_buffer[i+1]<<16) |
                    (input_buffer[i+2]<< 8) |
                    (input_buffer[i+3]<< 0);
        
        // decrypt chunk
        chunk = swap_bits(chunk, DECRYPT_BITS);
        chunk ^= 0x8b8b8b8b;
        
        // output 'decrypted' chunk
        output_buffer[i+0] = (chunk>>24);
        output_buffer[i+1] = (chunk>>16);
        output_buffer[i+2] = (chunk>> 8);
        output_buffer[i+3] = (chunk>> 0);
    }
}

uint8_t* read_hex_array(const char* hex_str, int& len)
{
    int l = strlen(hex_str);
    if (l & 1) {
        puts("Length of hex array must be even");
        return nullptr;
    }

    len = l / 2;
    uint8_t* res = new uint8_t[len];
    for (int i = 0; i < len; i++) {
        int b;
        if (sscanf(&hex_str[i * 2], "%02x", &b) <= 0) {
            delete res;
            return nullptr;
        }
        res[i] = b;
    }
    return res;
}

int main(int argc, const char* argv[])
{
    if(argc != 5) {
        puts("Usage: FirmPatch <offset> <hex bytes> <input file> <output file>");
        return 1;
    }

    uint32_t patch_offset;
    if (sscanf(argv[1], "%x", &patch_offset) <= 0) {
        puts("Invalid patch offset");
        return 1;
    }

    int patch_data_len;
    uint8_t* patch_data = read_hex_array(argv[2], patch_data_len);
    if (!patch_data) {
        puts("Invalid patch data");
        return 1;
    }

    // open files
    const char* strInputFilename = argv[3];
    const char* strOutputFilename = argv[4];
    FILE* fdInput  = fopen(strInputFilename,  "rb");
    if (fdInput == NULL) {
        printf("Error opening input file %s\n", strInputFilename);
        return 2;
    }

    // buffer input file
    fseek(fdInput, 0, SEEK_END);
    int iFileSize = ftell(fdInput);
    fseek(fdInput, 0, SEEK_SET);
    uint8_t* inputBuffer = new uint8_t[iFileSize];
    uint8_t* outputBuffer = new uint8_t[iFileSize];
    memset(outputBuffer, 0, iFileSize);
    fread(inputBuffer, 1, iFileSize, fdInput);

    if (iFileSize < patch_offset + patch_data_len) {
        puts("Illegal offset (beyond file size)");
        return 1;
    }
    if (iFileSize != 0x40000) {
        puts("Invalid flash size (expected 256K)");
        return 1;
    }

    // decrypt
    decrypt_firmware(inputBuffer, outputBuffer, iFileSize);

    // patch
    for (unsigned i = patch_offset; i < patch_offset + patch_data_len; i++) {
        outputBuffer[i] = patch_data[i - patch_offset];
    }

    // recalc checksum
    uint32_t checksum = 0;
    for (int i = 0x6000; i < 0x3E7FC; i += 4) {
        checksum += *(uint32_t*)&outputBuffer[i];
    }
    checksum = ~checksum + 1;
    *(uint32_t*) &outputBuffer[0x3E7FC] = checksum;

    // encrypt
    encrypt_firmware(outputBuffer, outputBuffer, iFileSize);

    // write output
    FILE* fdOutput = fopen(strOutputFilename, "w+b");
    if (fdOutput == NULL) {
        fclose(fdInput);
        printf("Error creating output file %s\n", strInputFilename);
        return 2;
    }
    fwrite(outputBuffer, 1, iFileSize, fdOutput);

    // finish up
    delete patch_data;
    fclose(fdInput);
    fclose(fdOutput);
    printf("done\n");
    return 0;
}
