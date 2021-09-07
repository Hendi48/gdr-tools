#pragma once

#include <device/ata.h>
#include <machine/scsi.h>
#include <util/rtl.h>

#pragma pack(push,1)
typedef struct {
    uint8_t PageCode : 6; // 0x3E
    uint8_t Reserved : 1;
    uint8_t PSBit : 1;
    uint8_t PageLength;

    uint8_t PartitionArea;
    uint8_t CDFValid;
    uint8_t Authentication;

    uint8_t DiscCategoryAndVersion;
    uint8_t DrivePhaseLevel;
    uint8_t ChallengeID;
    uint32_t ChallengeValue;
    uint32_t ResponseValue;
    uint32_t Reserved2;
} XGD_AUTHENTICATION_PAGE, *PXGD_AUTHENTICATION_PAGE;

typedef struct {
    MODE_PARAMETER_HEADER10 Header;
    XGD_AUTHENTICATION_PAGE AuthenticationPage;
} XGD_AUTHENTICATION, *PXGD_AUTHENTICATION;

static_assert(sizeof(XGD_AUTHENTICATION) == 28);

typedef struct {
    uint8_t ChallengeLevel;
    uint8_t ChallengeID;
    uint32_t ChallengeValue;
    uint8_t ResponseModifier;
    uint32_t ResponseValue;
} XGD_HOST_CHALLENGE_RESPONSE_ENTRY, *PXGD_HOST_CHALLENGE_RESPONSE_ENTRY;

#define XGD_HOST_CHALLENGE_RESPONSE_ENTRY_COUNT 23

typedef struct {
    uint8_t Version;
    uint8_t NumberOfEntries;
    XGD_HOST_CHALLENGE_RESPONSE_ENTRY Entries[XGD_HOST_CHALLENGE_RESPONSE_ENTRY_COUNT];
} XGD_HOST_CHALLENGE_RESPONSE_TABLE, *PXGD_HOST_CHALLENGE_RESPONSE_TABLE;

typedef uint8_t GUID[16];

typedef struct {
    uint8_t Length[2];
    uint8_t Reserved[2];
    DVD_LAYER_DESCRIPTOR LayerDescriptor;
    uint8_t Reserved2[15];
    uint8_t Reserved3[736];
    XGD_HOST_CHALLENGE_RESPONSE_TABLE HostChallengeResponseTable;
    uint8_t Reserved4[32];
    uint64_t ContentSourceTimeStamp;
    uint8_t Reserved5[20];
    GUID ContentSourceIdentifier;
    uint8_t Reserved6[84];
    uint64_t AuthoringTimeStamp;
    uint8_t Reserved7[19];
    uint8_t AuthoringSystemType;
    GUID AuthoringSystemIdentifier;
    uint8_t AuthoringHash[20];
    uint8_t AuthoringSignature[256];
    uint64_t MasteringTimeStamp;
    uint8_t Reserved8[19];
    uint8_t MasteringSystemType;
    GUID MasteringSystemIdentifier;
    uint8_t MasteringHash[20];
    uint8_t MasteringSignature[64];
    uint8_t Reserved9;
} XGD_CONTROL_DATA, *PXGD_CONTROL_DATA;
#pragma pack(pop)

static_assert(sizeof(XGD_CONTROL_DATA) == 1636);
static_assert(offsetof(XGD_CONTROL_DATA, AuthoringHash) - offsetof(XGD_CONTROL_DATA, AuthoringTimeStamp) == 44);


class XboxAuth {
private:
    ATADevice& m_device;

    void decrypt_host_challenge_table(PXGD_CONTROL_DATA ctl);
    bool issue_challenge(PXGD_CONTROL_DATA ctl, PXGD_HOST_CHALLENGE_RESPONSE_ENTRY ent, bool is_first, bool is_last);
public:
    XboxAuth(ATADevice& device) : m_device(device) {}

    void authenticate_disc();
};
