#include "user/xbox.h"
#include "device/cgastr.h"
#include "interrupt/secure.h"

#include "util/rtl.h"
#include "util/rc4.h"
#include "util/sha1.h"

#include <cinttypes>

#define CONTROL_DATA_LAYER 1
#define CONTROL_DATA_BLOCK_NUM 0xFD0200

void XboxAuth::authenticate_disc()
{
	// Request Authentication Page
	uint8_t command[12] = {SCSI_MODE_SENSE10, 0, 0x3E, 0, 0, 0, 0, 0 /*hi*/, 28 /*lo*/, 0};
	m_device.send_packet(command, 28);
	{
		Secure sec;
		m_device.wait_for_interrupt();
	}

	m_device.poll_request_ready();
	if (m_device.has_error()) {
		kout << "Error: " << hex << m_device.get_error_register() << dec << endl;
		return;
	}

	XGD_AUTHENTICATION auth;
	unsigned char* buf = (unsigned char*) &auth;
	m_device.read_buffer(buf, 14);
	for (int i = 0; i < 8; i++) {
		kout << hex << (int) buf[i] << dec << " ";
	}
	kout << endl;
	for (int i = 8; i < 28; i++) {
		kout << hex << (int) buf[i] << dec << " ";
	}
	kout << endl;
	kout << "CDFValid: " << (int) auth.AuthenticationPage.CDFValid << endl;
	kout.flush();

	if (!auth.AuthenticationPage.CDFValid) {
		return;
	}

	// Request Control Block
	REISSUE_CTL:
	uint8_t command_ctl[12] = {SCSI_READ_DVD_STRUCTURE, 0,
		((uint32_t)~CONTROL_DATA_BLOCK_NUM >> 24) & 0xFF,
		((uint32_t)~CONTROL_DATA_BLOCK_NUM >> 16) & 0xFF,
		((uint32_t)~CONTROL_DATA_BLOCK_NUM >> 8) & 0xFF,
		(uint32_t)~CONTROL_DATA_BLOCK_NUM & 0xFF,
		(uint8_t)~CONTROL_DATA_LAYER,
		0 /* physical descriptor */,
		sizeof(XGD_CONTROL_DATA) >> 8 /*hi*/,
		sizeof(XGD_CONTROL_DATA) & 0xFF /*lo*/,
		0,
		0xC0 /* control (proprietary) */};
	m_device.send_packet(command_ctl, sizeof(XGD_CONTROL_DATA));
	/*{
		Secure sec;
		m_device.wait_for_interrupt();
	}*/

	m_device.poll_request_ready();
	if (m_device.has_error()) {
		kout << "Error: " << hex << m_device.get_error_register() << dec << endl;
		return;
	}

	XGD_CONTROL_DATA ctl;
	if (!m_device.read_data((uint8_t*) &ctl, sizeof(XGD_CONTROL_DATA) / 2)) {
		for (int d = 0; d < 1000000; d++) m_device.get_status_register();
		goto REISSUE_CTL;
	}

	// Wait for pending interrupts
	for (int d = 0; d < 100000; d++) m_device.get_status_register();

	if (((ctl.LayerDescriptor.BookType << 4) | ctl.LayerDescriptor.BookVersion) == auth.AuthenticationPage.DiscCategoryAndVersion) {
		kout << "[+] BookType and BookVersion match" << endl;
	} else {
		kout << "[-] BookType and BookVersion mismatch!" << endl;
	}

	decrypt_host_challenge_table(&ctl);
	for (int i = 10; i < 23; i++) {
		for (int j = 0; j < 11; j++) {
			kout << hex << (int) ((unsigned char*)&ctl.HostChallengeResponseTable.Entries[i])[j] << dec << " ";
		}
		kout << endl;
	}
	kout.flush();

	int idx = -1;
	bool first = true;
	for (int i = 0; i < XGD_HOST_CHALLENGE_RESPONSE_ENTRY_COUNT; i++) {
		if (ctl.HostChallengeResponseTable.Entries[i].ChallengeLevel == 1) {
			if (!issue_challenge(&ctl, &ctl.HostChallengeResponseTable.Entries[i], first, false)) {
				kout << "Challenge " << i << " failed" << endl;
				return;
			}
			first = false;
			idx = i;
		}
	}

	if (idx == -1) {
		kout << "Couldn't find proper challenge entry" << endl;
		return;
	}

	if (!issue_challenge(&ctl, &ctl.HostChallengeResponseTable.Entries[idx], false, true)) {
		kout << "Final challenge failed" << endl;
		return;
	}

	kout << "Authentication successful" << endl;
}

void XboxAuth::decrypt_host_challenge_table(PXGD_CONTROL_DATA ctl)
{
	SHA1_CTX sha;
	rc4_state rc4;
	unsigned char sha_digest[20];

	// kout << ctl->AuthoringTimeStamp << endl;
	// kout.flush();

	SHA1Init(&sha);
	SHA1Update(&sha, (unsigned char*)&ctl->AuthoringTimeStamp,
		offsetof(XGD_CONTROL_DATA, AuthoringHash) - offsetof(XGD_CONTROL_DATA, AuthoringTimeStamp));
	SHA1Final(sha_digest, &sha);

	rc4_init(&rc4, sha_digest, 7);
	rc4_crypt(&rc4, (uint8_t*)&ctl->HostChallengeResponseTable.Entries,
		(uint8_t*)&ctl->HostChallengeResponseTable.Entries,
		sizeof(ctl->HostChallengeResponseTable.Entries));
}

bool XboxAuth::issue_challenge(PXGD_CONTROL_DATA ctl, PXGD_HOST_CHALLENGE_RESPONSE_ENTRY ent,
	bool is_first, bool is_last)
{
	XGD_AUTHENTICATION auth;

	memset(&auth, 0, sizeof(auth));
	auth.Header.ModeDataLength[1] = 26;
	auth.AuthenticationPage.PageCode = 0x3E;
	auth.AuthenticationPage.PageLength = 18;
	auth.AuthenticationPage.CDFValid = 1;
	auth.AuthenticationPage.DiscCategoryAndVersion = (ctl->LayerDescriptor.BookType << 4) | ctl->LayerDescriptor.BookVersion;
	auth.AuthenticationPage.DrivePhaseLevel = 1;

	auth.AuthenticationPage.ChallengeID = ent->ChallengeID;
	auth.AuthenticationPage.ChallengeValue = ent->ChallengeValue;

	if (!is_first) {
		auth.AuthenticationPage.Authentication = 1;
	}
	if (is_last) {
		auth.AuthenticationPage.PartitionArea = 1;
	}

	for (int d = 0; d < 100000; d++) m_device.get_status_register();

	// Challenge
	uint8_t command_select[12] = {SCSI_MODE_SELECT10, 0, 0, 0, 0, 0, 0, 0 /*hi*/, 28 /*lo*/, 0};
	m_device.send_packet(command_select, 28);
	{
		Secure sec;
		m_device.wait_for_interrupt();
	}
	m_device.poll_request_ready();

	if (m_device.has_error()) {
		return false;
	}
	if (m_device.write_buffer((uint8_t*) &auth, 14) != 14) {
		kout << "Incomplete write!" << endl;
	}
	{
		Secure sec;
		m_device.wait_for_interrupt();
	}

	if (m_device.has_error()) {
		kout << "Error: " << hex << m_device.get_error_register() << dec << endl;
		kout.flush();

		SENSE_DATA sense;
		if (m_device.request_sense(&sense)) {
			if (sense.IncorrectLength) {
				kout << "Incorrect block length";
			} else {
				kout << "Sense: " << hex << (int) sense.SenseKey << "; ASC: " << (int) sense.AdditionalSenseCode << dec << endl;
			}
			kout.flush();
		} else {
			kout << "Sense req failed" << endl;
			kout.flush();
		}

		return false;
	}

	// Response
	uint8_t command_sense[12] = {SCSI_MODE_SENSE10, 0, 0x3E, 0, 0, 0, 0, 0 /*hi*/, 28 /*lo*/, 0};
	m_device.send_packet(command_sense, 28);
	{
		Secure sec;
		m_device.wait_for_interrupt();
	}

	m_device.poll_request_ready();
	if (m_device.has_error()) {
		kout << "Error: " << hex << m_device.get_error_register() << dec << endl;
		kout.flush();
		return false;
	}

	memset(&auth, 0, sizeof(auth));
	unsigned char* buf = (unsigned char*) &auth;
	m_device.read_buffer(buf, 14);
	/*for (int i = 0; i < 8; i++) {
		kout << hex << (int) buf[i] << dec << " ";
	}
	kout << endl;
	for (int i = 8; i < 28; i++) {
		kout << hex << (int) buf[i] << dec << " ";
	}
	kout << endl;
	kout.flush();*/

	return auth.AuthenticationPage.ResponseValue == ent->ResponseValue;
}
