#include "BearLDC.h"
#include <thread>  


// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

//// EchoHandler ///////////////////////////////////////////////////////
// Handles the incoming data by reflecting it back to the sender.

void *
ConnHandler(void* client)
{
	int nRetval = 0;
	BearLDC *Client = (BearLDC *)client;

	pthread_t ThreadID = pthread_self();


//	Client->bear_print((TCHAR *)L"\nThread Id In thread -> %lx\n", ThreadID.p);
	try {
		BearLDC *Client = (BearLDC *)client;


	}
	catch (...)
	{

	}

	Client->ConnectList.RemoveThread(ThreadID);

	return NULL;
}


//// SetUpListener /////////////////////////////////////////////////////
// Sets up a listener on the given interface and port, returning the
// listening socket if successful; if not, returns INVALID_SOCKET.

SOCKET
SetUpListener(USHORT nPort)
{
	try {
		SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
		if (sd != INVALID_SOCKET)
		{
			sockaddr_in sinInterface;
			sinInterface.sin_family = AF_INET;
			sinInterface.sin_addr.s_addr = INADDR_ANY;
			sinInterface.sin_port = nPort;
			int ret = (int)::bind(sd, (sockaddr*)&sinInterface, sizeof(sockaddr_in));

			if (ret != SOCKET_ERROR)
			{
				listen(sd, SOMAXCONN);
				return sd;
			}

		}
	}
	catch (...)
	{
		return INVALID_SOCKET;
	}

	return INVALID_SOCKET;
}

//// AcceptConnections /////////////////////////////////////////////////
// Spins forever waiting for connections.  For each one that comes in, 
// we create a thread to handle it and go back to waiting for
// connections.  If an error occurs, we return.

int
AcceptConnections(BearLDC *Client, SOCKET sock)
{
	sockaddr_in sinRemote;

	pthread_t ThreadID;
	int nAddrSize = sizeof(sinRemote);

	try {
		while (1)
		{


			SOCKET sd = accept(sock, (sockaddr*)&sinRemote, &nAddrSize);
			if (sd != INVALID_SOCKET)
			{
				Client->bear_print((TCHAR *)L"\nAccepted connection from %ws %u", inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port));

				pthread_create(&ThreadID, NULL, ConnHandler, (void *)Client);
				Client->ConnectList.AddThread(ThreadID);
			}
			else
			{
				Client->bear_print((TCHAR *)L"\naccept() failed\n");
				return -2;
			}
		}
	}
	catch (...)
	{
		return -3;
	}

	return 0;
}

void *
DoWinsock(void* client)
{
	WSADATA wsaData;

	BearLDC *Client = (BearLDC *)client;

	try {

		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);


		if (iResult != NO_ERROR) 
		{
			Client->bear_print((TCHAR *)"Error at WSAStartup()\n");
			return NULL;
		}

		cout << "Establishing the listener..." << endl;
		SOCKET ListeningSocket = SetUpListener(htons(Client->oobPort));
		if (ListeningSocket == INVALID_SOCKET)
		{
			Client->bear_print((TCHAR *)L"Error: establish listener\n");
			return NULL;
		}

		Client->bear_print((TCHAR *)L"Waiting for connections...\n");
		while (1)
		{
			AcceptConnections(Client, ListeningSocket);
			Client->bear_print((TCHAR *)L"Listener restarting...\n");
		}


		WSACleanup();
	}
	catch (...)
	{
		return NULL;
	}

	return NULL;   // warning eater

}


#if 1
void
dumpList(BearLDC *Client)
{
	struct connect_threads *ptr;
	ptr = Client->ConnectList.thead;

	while (ptr != NULL)
	{
		Client->bear_print((TCHAR *) L"ptr=%lx %lx\n", ptr->ThreadID, ptr->ThreadID.p);
		ptr = ptr->next;
	}
}

int
TestThread(void* client)
{
	pthread_t ThreadID;
	BearLDC *Client = (BearLDC *)client;
	for (int k = 0; k < 20; k++)
	{
		pthread_create(&ThreadID, NULL, ConnHandler, (void *)Client);
		Client->ConnectList.AddThread(ThreadID);
		k = k;
	}
	//Client->bear_print((TCHAR *)L"\nThread After -> %lx\n", ThreadID.p);
	dumpList(Client);
	dumpList(Client);

	return 0;
}
#endif