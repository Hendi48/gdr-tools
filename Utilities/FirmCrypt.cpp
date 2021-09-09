// 
// tool to decrypt/encrypt dvd firmware files from the xbox360 dvd drive
// 
// this tool takes and existing input file and either encrypts or decrypts
// it and writes the output to a new file
// 
// based on code by mAreK
// 
// december 2005 - loser
// 

#include <stdio.h>
#include <string.h>

typedef unsigned int    u32;
typedef unsigned char   u8;

const char title[] =    "FirmCrypt v0.1 - loser 2005";
const char usage[] =    "Usage: FirmCrypt <option> <input filename> <output filename>\n"
                        "       option: d = decrypt file\n"
                        "               e = encrypt file";


u32 swapBits(u32 chunk, int* bits)
{
    u32 result = 0;
    
    int i;
    for(i=0; i < 32; i++)
    {
        u32 bit = (chunk & (1 << bits[i])) >> bits[i];
        result = (result << 1) | bit;
    }
    
    return result;

}

void encryptFirmware(u8* inputBuffer, u8* outputBuffer, int size)
{
    int encryptBits[32] = { 22, 27,  3, 10, 14, 16,  1, 28,
                            26,  7, 15, 21,  5, 19, 29, 12,
                             2, 11, 24, 20, 13, 18,  4, 30,
                             9, 17, 23,  0,  6, 31,  8, 25 };
    
    // process each 32bit chunk
    for(int i=0; i<size; i+=4)
    {
        // get 'decrypted' chunk
        u32 chunk = (inputBuffer[i+0]<<24) |
                    (inputBuffer[i+1]<<16) |
                    (inputBuffer[i+2]<< 8) |
                    (inputBuffer[i+3]<< 0);
        
        // encrypt chunk
        chunk ^= 0x8b8b8b8b;
        chunk = swapBits(chunk, encryptBits);
        
        // output 'encrypted' chunk
        outputBuffer[i+0] = (chunk>>24);
        outputBuffer[i+1] = (chunk>>16);
        outputBuffer[i+2] = (chunk>> 8);
        outputBuffer[i+3] = (chunk>> 0);
    }
}

void decryptFirmware(u8* inputBuffer, u8* outputBuffer, int size)
{
    int decryptBits[32] = {  2,  8, 17, 24, 30, 23,  0, 13,
                             5, 31, 20, 12, 18, 10,  6, 26,
                            21, 27, 11, 16, 14, 28,  7,  1,
                            22,  3, 19,  9, 29, 15, 25,  4 };
    
    // process each 32bit chunk
    for(int i=0; i<size; i+=4)
    {
        // get 'encrypted' chunk
        u32 chunk = (inputBuffer[i+0]<<24) |
                    (inputBuffer[i+1]<<16) |
                    (inputBuffer[i+2]<< 8) |
                    (inputBuffer[i+3]<< 0);
        
        // decrypt chunk
        chunk = swapBits(chunk, decryptBits);
        chunk ^= 0x8b8b8b8b;
        
        // output 'decrypted' chunk
        outputBuffer[i+0] = (chunk>>24);
        outputBuffer[i+1] = (chunk>>16);
        outputBuffer[i+2] = (chunk>> 8);
        outputBuffer[i+3] = (chunk>> 0);
    }
}

int main(int argc, const char* argv[])
{
    // print title and check args
    printf("%s\n", title);
    if(argc != 4)
    {
        printf("%s\n", usage);
        return 1;
    }
    
    // check whether encrypting or decrypting
    bool encryptFile;
    const char* option = argv[1];
    if(option[0] == 'd' || option[0] == 'D')
        encryptFile = false;
    else if(option[0] == 'e' || option[0] == 'E')
        encryptFile = true;
    else
    {
        printf("%s\n", usage);
        return 1;
    }
    
    // open files
    const char* strInputFilename = argv[2];
    const char* strOutputFilename = argv[3];
    FILE* fdInput  = fopen(strInputFilename,  "rb");
    if(fdInput == NULL)
    {
        printf("Error opening input file %s\n", strInputFilename);
        return 2;
    }
    FILE* fdOutput = fopen(strOutputFilename, "w+b");
    if(fdOutput == NULL)
    {
        fclose(fdInput);
        printf("Error creating output file %s\n", strInputFilename);
        return 2;
    }
    
    // buffer input file
    fseek(fdInput, 0, SEEK_END);
    int iFileSize = ftell(fdInput);
    fseek(fdInput, 0, SEEK_SET);
    u8* inputBuffer = new u8[iFileSize];
    u8* outputBuffer = new u8[iFileSize];
    memset(outputBuffer, 0, iFileSize);
    fread(inputBuffer, 1, iFileSize, fdInput);
    
    // encrypt/decrypt file
    if(encryptFile)
        encryptFirmware(inputBuffer, outputBuffer, iFileSize);
    else
        decryptFirmware(inputBuffer, outputBuffer, iFileSize);
    
    // write out resultant output
    fwrite(outputBuffer, 1, iFileSize, fdOutput);
    
    // finish up
    fclose(fdInput);
    fclose(fdOutput);
    printf("done\n");
    return 0;
}
