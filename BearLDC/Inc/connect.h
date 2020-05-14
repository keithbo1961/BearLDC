#pragma once
#include "BearLDC.h"
#include "aes.h"

/*Comm Commands*/
typedef enum
{
	NO_ACTION = 0,   // All clear
	START,           // Start hub/sensor
	STOP,            // Stop hub/sensor
	DEFAULT_POLICY,  // Reset Default policy.
	NEW_POLICY,      // New Policy (New policy ID will be in the policy/parameter field)
	RE_KEY,          // Re-Key connection.
	PULL_POLICY,     // Pull new hub policy set. (available to Hub only)
	AUTHORIZE,       // Authorize Sensor.
	DENY_AUTH,       // Authorization Denied.
	DE_AUTHORIZE,    // De-authorize a Sensor (TBD: new port/policy information will be in the .cmd policy/parameter field)
	REMOVE,          // Remove Device from cloud. (Special Case: Can be issued from either Connection List or Hub)
	RENAME,          // Rename the device. (New name will be in the .cmd policy/parameter field)  RESERVED_1,
	BLOCK,
	UNBLOCK,
	CONFIG,
	HEARTBEAT,
	RESERVED_1,      // Recommend leaving 10 slots for Cloud initiated Device Configuration Data (Currently there are 7 including Rename)
	RESERVED_2,
	RESERVED_3,
	RESERVED_4,
	RESERVED_5,
	RESERVED_6,
	RESERVED_7,
	RESERVED_8,
	RESERVED_9,
	RESERVED_10
} Command;


/* Comm Commands */
typedef enum
{
	P_CONNECTED = 0x0000,    // Connected State, No actions pending.
	P_REKEY_SENSOR,          // Sensor is initializing key state for connection.
	P_REKEY_HUB,             // Hub is initializing key state for connection. .
	P_REKEY_INIT,            // Hub has initiated a Re-Key sequence
	P_POLICYID,              // Hub has initiated a policy id change sequence.
	P_POLICY_FILE,           // Hub has initiated a policy file change sequence.
	P_DEFAULT_FILE,          // Hub has initiated a default policy file reset sequence.
	P_CONNECT_SEND_FAIL,     // Hub/Sensor failed on OOB send command.
	P_CONNECT_RCV_FAIL,      // Hub/Sensor failed on OOB receive command.
	P_CONNECTION_TIMEOUT,    // OOB connection timed out.
	P_SHUTDOWN,              // OOB Close state.
	P_STARTUP,               // Encryption Engine is keying.
	P_OFFLINE,               // Hub/Sensor is offline.
	P_AUTH_PENDING,          // A new connection is waiting for authorization.
	P_AUTH_B1,               // New Bear connection key sent.
	P_AUTH_B2,               // Second bear key auth sent.                
	P_AUTH_ACCEPT,           // Authorization is accepted.
	P_AUTH_DENIED,           // Authorization denied.
	P_RENAME,                // Rename Sensor
	P_LOST_CONNECT,          // Lost Connection.
	P_DEVICE_BUSY,
	P_DATA_STOPPED,          // Data channel stopped.
	P_DATA_RATE_HIGH,        // Data channel rate above policy training specs.
	P_DATA_RATE_LOW,         // Data channel rate below policy training specs.
	P_DATA_BLOCK,            // Data channel blocked.
	P_DATA_ISOLATE,            // Data channel blocked.

} CONNECT_STATE;




/* Connection IP state node. */
struct connect_struct
{
	char DevID[640];
	unsigned int ip;
	int Commport;
	int state;
	char HostName[128];
	char version[128];
	int  DeviceType;
	time_t stime;
	time_t etime;
	int isKey;
	int isAuth;
	int failCnt;
	int Block;
	unsigned long LastCnt;
	unsigned long CurCnt;
	int hittimes;

	mbedtls_aes_context ctx;
	mbedtls_aes_context oobctx;
	SIKE_DH keys;
	struct connect_struct *next;
};


/* Port node.*/
struct connect_ports
{
	unsigned short port;
	int            Block;
	struct connect_ports *next;
};

/* thread node.*/
struct connect_threads
{
	pthread_t ThreadID;
	time_t stime;
	struct connect_threads *next;
};


/* Structure to hold pointers to the linked lists for connetions/ports for pfilter */
typedef struct
{
	int isHub;
	struct connect_struct *head;
	struct connect_ports  *phead;
} CommState;





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
} pError;

/* Threaded socket receive data structure */
typedef struct {
	SOCKET        sock;
	pError        Ret;
	in_addr	   sin_addr;
	char Cmd[4096];
	char Msg[4096];
} PacketThread;



class CCL
{
public:

	pError AddConnection(unsigned int ip, char *DevID);
	pError RemoveConnection(unsigned int ip);
	struct connect_struct *FindIP(unsigned int ip);
	struct connect_struct *FindID(char *DevID);

	pError AddPort(unsigned short port, int Block);
	pError RemovePort(unsigned short port);
	struct connect_ports *FindPort(unsigned short port);


	pError AddThread(pthread_t val);
	pError RemoveThread(pthread_t val);
	struct connect_threads *FindThread(pthread_t val);

//private:
	struct connect_struct *head = NULL;
	int ccnt;

	struct connect_ports *phead = NULL;
	int pcnt;

	struct connect_threads *thead = NULL;
	int tcnt;


	pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

};

