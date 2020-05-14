#pragma once

#include "resource.h"

#include "framework.h"
#include "resource.h"
#include <tlhelp32.h>
#include "aes.h"
#include "P751_api.h"
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/http_client.h"
#include "cpprest/filestream.h"

#include <io.h>
#include <time.h>

#include "../Inc/AKESIKE.h"
#include "../Inc/P751_api.h"
#include "aes.h"
#include "pthread/pthreadlib.h"
#include <winsock.h>
#include "BearCmd.h"
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



using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace std;

using namespace http::experimental::listener;

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



class Apps
{
	string_t Name;
	string_t Ip;
	string_t ports;
};

class BlockIP
{
	vector <string_t> IP;
	vector <DWORD> addr;
};

class BlockPort
{
	string_t Port;
};

class BlockApp
{
	string_t App;
	bool     Remove;
};


class ConnectList
{
	string_t IP;
	string_t Port;
};




class BearLDC
{
public:
	BearLDC();

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

	CCL ConnectList;

	vector<string_t>BlockIPs;
	vector<DWORD>BlockIPa;

	vector<string_t>EncryptIPs;
	vector<DWORD>EncryptIPa;

	vector<USHORT> AllowPorts;
	vector<USHORT> BlockPorts;
	vector<string_t> BlockApptList;
	vector<Apps*> AppList;

	bool     LogToFile;

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

	void     PING();

	void     ALERT(int Alert, string_t msg);
	void     Echo(string_t cmd);
	void     Register();
	void     Updated();

	void     bear_print(WCHAR    *fmt, ...);
	void     bear_print(CHAR    *fmt, ...);
	DWORD    ExecProc(TCHAR *cmdline);
	DWORD    FindProcessId(const WCHAR *processname);


	unsigned char PrivateKeyA[SIDH_SECRETKEYBYTES_A];
	unsigned char PrivateKeyB[SIDH_SECRETKEYBYTES_B];
	unsigned char PublicKeyA[SIDH_PUBLICKEYBYTES];
	unsigned char PublicKeyB[SIDH_PUBLICKEYBYTES];

	BEARCMD BearCmd;
};





#ifdef MAIN_MODULE
#define EXTERN
#else
#define EXTERN extern
#endif
#include <mutex>

EXTERN std::mutex mtx;


const string_t Encrypt = L"/ProtectIPs";
const string_t Block = L"/BlockedIPs";
const string_t Ports = L"/AllowPorts";
const string_t bPorts = L"/BlockPorts";
const string_t Power = L"/PowerCycle";
const string_t Kill = L"/KillProcess";
const string_t Host = L"/ChangeDeviceName";
const string_t Ping = L"/Ping";
const string_t Echo = L"/Echo";
const string_t AlertS = L"/AlertType";
const string_t ScanProtect = L"/ScanProtect";

int StartFilter();
int StopFilter();
int SetPublicKeys(BearLDC *client);
int GetSharedKey(BearLDC *client, unsigned char *PublicKey, unsigned char *SharedSecret, int mode);
int Bas64Encode(unsigned char *in, char *out, int size);
int Base64Decode(char *in, unsigned char *out);
int ClientKeysInit(SIKE_DH *keys);
int ClientKeys(SIKE_DH *keys, unsigned char *msg, unsigned char *ctToLDC);
int MakeEncrpytIPSeed(unsigned char *key, int keylenkey, char * iv, mbedtls_aes_context *ctx);
int MangleEncryptOut(unsigned char *in, unsigned char *out, int len, mbedtls_aes_context *ctx);
int MangleDecryptIn(unsigned char *in, unsigned char *out, int len, mbedtls_aes_context *ctx);

