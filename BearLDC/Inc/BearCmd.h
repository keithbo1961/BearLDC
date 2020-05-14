#pragma once

#include "aes.h"

/* Comm Commands */
typedef enum
{
	P_DISCOVER = 0xf000,     // Initial Command sent to HUB.
	P_NEW_POLICY_FILE,       // Command to push/pull new policy file (policy file included in n number of buffers).
	P_POLICY_ID,             // Force Policy ID change.
	P_DH_CRK,                // Diffie-Hellman key exchange data of Client key (sent to server)
	P_DH_SRK,                // Diffie-Hellman key exchange data of Server key (sent to client)
	P_CLOSE,                 // Force Sensor to close pFilter.
	P_RESET,                 // Force Sensor code reset.
	P_OK,                    // Command recieved with good status.
	P_BAD_CRC,               // CRC failure on receive.
	P_BAD_DATA,              // Sent if Command specific data does not match.
	P_HBEAT,				   // Sent as heartbeat to sensor
	P_REKEY,
	P_DEFAULT_POLICY_FILE,
	P_CONFIG,
	P_START
} BEARACTION;

/* Comm Status Enumeration */
typedef enum
{
	STATUS_OK,
	CRC_ERROR,              // Sent if Command specific data does not match.
	BAD_COMMAND,
	MALLOC_ERROR,
	SOCKET_FAIL,
	SOCKET_BAD_HOSTNAME,
	SOCKET_CONNECT_FAIL,
	SOCKET_READ_FAIL,
	SOCKET_READ_TIMEOUT,
	SOCKET_WRITE_FAIL,
	CONNECTION_NOT_FOUND,
	THREAD_ERROR,
	EXCEPTION,
} BEARREPLY;

typedef struct {
	BYTE ID[4];
	CHAR Version[8];
	BYTE Hash[4];
	BEARACTION BearAction;
	USHORT len;
	BYTE *Buf;
} CMD;

typedef struct {
	BYTE ID[4];
	CHAR Version[8];
	BYTE Hash[4];
	BEARREPLY BearReply;
	USHORT len;
	BYTE *Buf;
} REPLY;

typedef struct {

	BYTE nounce[8];
	CMD Cmd;

} CmdWrap;

typedef struct {

	BYTE nounce[8];
	REPLY Cmd;

} ReplyWrap;

class BEARCMD {

	CmdWrap Command;

};