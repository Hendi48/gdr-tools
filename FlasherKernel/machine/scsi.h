#pragma once

#include <cinttypes>

#define SCSI_REQUEST_SENSE 0x03
#define SCSI_INQUIRY 0x12
#define SCSI_READ_CAPACITY 0x25
#define SCSI_READ 0x28
#define SCSI_MODE_SELECT10 0x55
#define SCSI_MODE_SENSE10 0x5A
#define SCSI_READ_DVD_STRUCTURE 0xAD

typedef struct
{
    uint8_t ModeDataLength[2];
    uint8_t MediumType;
    uint8_t DeviceSpecificParameter;
    uint8_t Reserved[2];
    uint8_t BlockDescriptorLength[2];
} MODE_PARAMETER_HEADER10, *PMODE_PARAMETER_HEADER10;

typedef struct
{
    uint8_t ErrorCode : 7;
    uint8_t Valid : 1;
    uint8_t SegmentNumber;
    uint8_t SenseKey : 4;
    uint8_t Reserved : 1;
    uint8_t IncorrectLength : 1;
    uint8_t EndOfMedia : 1;
    uint8_t FileMark : 1;
    uint8_t Information[4];
    uint8_t AdditionalSenseLength;
    uint8_t CommandSpecificInformation[4];
    uint8_t AdditionalSenseCode;
    uint8_t AdditionalSenseCodeQualifier;
    uint8_t FieldReplaceableUnitCode;
    uint8_t SenseKeySpecific[3];
} SENSE_DATA, *PSENSE_DATA;

typedef struct
{
    uint32_t LogicalBlockAddress;
    uint32_t BytesPerBlock;
} READ_CAPACITY_DATA, *PREAD_CAPACITY_DATA;

#pragma pack(push, 1)
typedef struct
{
    uint8_t BookVersion : 4;
    uint8_t BookType : 4;
    uint8_t MinimumRate : 4;
    uint8_t DiskSize : 4;
    uint8_t LayerType : 4;
    uint8_t TrackPath : 1;
    uint8_t NumberOfLayers : 2;
    uint8_t Reserved1 : 1;
    uint8_t TrackDensity : 4;
    uint8_t LinearDensity : 4;
    uint32_t StartingDataSector;
    uint32_t EndDataSector;
    uint32_t EndLayerZeroSector;
    uint8_t Reserved5 : 7;
    uint8_t BCAFlag : 1;
} DVD_LAYER_DESCRIPTOR, *PDVD_LAYER_DESCRIPTOR;
#pragma pack(pop)
