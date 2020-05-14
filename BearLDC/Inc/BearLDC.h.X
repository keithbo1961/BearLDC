#pragma once


#include "resource.h"

#ifdef _WIN32
#include <Windows.h>
#include <winsock.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>

#include "../Inc/AKESIKE.h"
#include "../Inc/P751_api.h"
#include "aes.h"
#include "pthreadlib.h"

using namespace std;

#if 1
typedef struct {



#ifdef BEARCLIENT
	unsigned char Clientsecret[CRYPTO_BYTES];
	unsigned char sikeprivate[CRYPTO_SECRETKEYBYTES];
	unsigned char sikepublic[CRYPTO_PUBLICKEYBYTES];
#else
	unsigned char LDCsecret[CRYPTO_BYTES];
	unsigned char LDC_sike_sk[CRYPTO_SECRETKEYBYTES];
	unsigned char LDC_part_ss[CRYPTO_BYTES];
#endif

	unsigned char aliceprivate[SIDH_SECRETKEYBYTES_A];
	unsigned char bobprivate[SIDH_SECRETKEYBYTES_B];

	unsigned char alicepublic[SIDH_PUBLICKEYBYTES];
	unsigned char bobpublic[SIDH_PUBLICKEYBYTES];

	BOOL isAlice;
} SIKE_DH;

#endif

#include "connect.h"

void DH(char *label, char *buf, int count);

int ClientKeysInit(SIKE_DH *keys);
int ClientKeys(SIKE_DH *keys, unsigned char *msg, unsigned char *ctToLDC);
int LDCkeysM1(SIKE_DH *keys, unsigned char *ClientPublicKey, unsigned char *msg);
int LDCkeysM2(SIKE_DH *keys, unsigned char *ctToLDC);
void bear_print(CHAR    *fmt, ...);
int Server(int Port);
int TestKeys();
int MakeEncrpytIPSeed(unsigned char *key, int keylenkey, char * iv, mbedtls_aes_context *ctx);
int MangleEncryptOut(unsigned char *in, unsigned char *out, int len, mbedtls_aes_context *ctx);
int MangleDecryptIn(unsigned char *in, unsigned char *out, int len, mbedtls_aes_context *ctx);

#ifdef MAIN_MODULE 
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN char  LogFile[_MAX_PATH];
EXTERN char  szVersionFile[MAX_PATH];
EXTERN int   LogToFile;




class BearLDC
{
public:
	string_t Version;
	string_t url;
	string_t DevID;
	string_t MyIp;
	string_t MyUrl;
	string_t MyPort;

	string_t HostName;
	string_t Location;

	string_t Dip;
	string_t Nip;

	string_t oobHost;
	USHORT   oobPort;

	int      MaxRetries;
	int      iniChange;
	int      isDriver;


	TCHAR    iniFile[MAX_PATH];
	TCHAR    TempDir[MAX_PATH];
	TCHAR    AppDir[MAX_PATH];
	TCHAR    CCLFile[MAX_PATH];
	TCHAR    PolicyFile[MAX_PATH];
	TCHAR    LogFile[MAX_PATH];

	time_t   StartTime;
	time_t   StopTime;
	UINT     Alert;
	int      AlertType;
	UINT     State;
	UINT     ListenState;
	int      Cnt;

	string_t EventCmd;
	time_t   EventTime;

	string_t AlertCmd;
	time_t   AlertTime;

	HWND     hConsole;


	void     sdi_print(WCHAR    *fmt, ...);
	void     sdi_print(CHAR    *fmt, ...);
};

#define BEAR_OK             0x0000
#define BEAR_NO_CONFIG_FILE 0x0001
#define BEAR_NO_TMPDIR      0x0002
#define BEAR_PRINTERS_FAIL  0x0004
#define BEAR_CONNECT_FAIL   0x0008
#define BEAR_AUTH_FAIL      0x0010
#define BEAR_SEND_FAIL      0x0020
#define BEAR_RCV_FAIL       0x0040
#define BEAR_JSON_P_FAIL    0x0080
#define BEAR_BAD_RESPONSE   0x0100
#define BEAR_ADDJOB_FAIL    0x0200
#define BEAR_UPDATEJOB_FAIL 0x0400
#define BEAR_GETJOBS_FAIL   0x0800
#define BEAR_OPTIONS_FAILED 0x1000

#define BEAR_LISTEN_OK        0x0000
#define BEAR_LISTEN_OFF       0x0001
#define BEAR_LISTEN_BIND_FAIL 0x0002
#define BEAR_LISTEN_BAD_REQ   0x0010
#define BEAR_LISTEN_RCV_EXP   0x0020

/* Logging Utiliy functions. */
void
BearLDC::sdi_print(CHAR    *fmt, ...)
{
	va_list     ap;
	CHAR        output_buffer[1024];


	std::ofstream f(LogFile, ios_base::app);


	va_start(ap, fmt);
	vsprintf_s(output_buffer, 1024, fmt, ap);

	if (!LogToFile)
	{
		OutputDebugStringA(output_buffer);
		va_end(ap);
		return;
	}

	if (f.is_open())
	{

		f << output_buffer;
	}
	else
	{
		OutputDebugStringA(output_buffer);
	}

	f.close();
	va_end(ap);
}

void
BearLDC::sdi_print(WCHAR    *fmt, ...)
{
	va_list     ap;
	WCHAR        output_buffer[1024];


	std::wofstream f(this->LogFile, ios_base::app);

	va_start(ap, fmt);
	vswprintf_s(output_buffer, 1024, fmt, ap);

	if (!LogToFile)
	{
		OutputDebugStringW(output_buffer);
		va_end(ap);
		return;
	}

	if (f.is_open())
	{

		f << output_buffer;
	}
	else
	{
		OutputDebugStringW(output_buffer);
	}

	f.close();
	va_end(ap);
}



