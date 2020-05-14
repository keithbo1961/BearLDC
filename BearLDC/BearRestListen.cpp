#include "framework.h"
#include "BearLDC.h"

class BEARSERVER
{
public:
	BEARSERVER() {}
	BEARSERVER(utility::string_t url, BearLDC *client);

	TCHAR    LogFile[MAX_PATH];


	pplx::task<void> open() { return m_listener.open(); }
	pplx::task<void> close() { return m_listener.close(); }

private:

	void handle_get(http_request message);
	void handle_put(http_request message);
	void handle_post(http_request message);
	void handle_delete(http_request message);

	BearLDC *client;
	http_listener m_listener;
};


std::unique_ptr<BEARSERVER> g_http;


void on_initialize(const string_t& address, BearLDC *client)
{
	// Build our listener's URI from the configured address and the hard-coded path "blackjack/dealer"

	uri_builder uri(address);
	//uri.append_path(U("sdiserver/action"));

	auto addr = uri.to_uri().to_string();
	g_http = std::unique_ptr<BEARSERVER>(new BEARSERVER(addr, client));
	try {

		g_http->open().wait();
		client->ListenState = BEAR_LISTEN_OK;
		client->bear_print((WCHAR *)U("Listening for requests at: "));
	}
	catch (const std::exception &e)
	{
		const char *t = e.what();

		client->bear_print((CHAR *)"\nException:(BearListen Failed) %s", t);
		client->ListenState |= BEAR_LISTEN_BIND_FAIL;
		int x = 1;
	}

	return;
}

void on_shutdown()
{
	g_http->close().wait();
	return;
}



BEARSERVER::BEARSERVER(utility::string_t url, BearLDC *Client) : m_listener(url)
{
	m_listener.support(methods::GET, std::bind(&BEARSERVER::handle_get, this, std::placeholders::_1));
	m_listener.support(methods::PUT, std::bind(&BEARSERVER::handle_put, this, std::placeholders::_1));
	m_listener.support(methods::POST, std::bind(&BEARSERVER::handle_post, this, std::placeholders::_1));
	m_listener.support(methods::DEL, std::bind(&BEARSERVER::handle_delete, this, std::placeholders::_1));
	client = Client;
}

//
// A GET
// 
void BEARSERVER::handle_get(http_request message)
{

	WCHAR ltm[1024];
	struct tm timeinfo;
	time(&client->EventTime);
	localtime_s(&timeinfo, &client->EventTime);


	client->ListenState &= ~BEAR_LISTEN_BAD_REQ;



	//	ucout << message.to_string() << std::endl;


	_wasctime_s(ltm, 1024, &timeinfo);

	try
	{

		string_t x = message.request_uri().to_string();
		client->bear_print((WCHAR*)L"GET  - %ws\n", x.c_str());


		if (x == L"/Ping")
		{
			client->EventCmd = L"Ping";
			client->ListenState = BEAR_LISTEN_OK;
			message.reply(status_codes::OK, L"Ok");
		}
		else
		{
			client->EventCmd = L"Unknown  " + x;
			client->ListenState |= BEAR_LISTEN_BAD_REQ;
			message.reply(status_codes::OK, L"Bad Request...");
		}
	}
	catch (...)
	{
		client->ListenState |= BEAR_LISTEN_RCV_EXP;
	}

};

//
// A POST 
// 
void BEARSERVER::handle_post(http_request message)
{
	client->ListenState &= ~BEAR_LISTEN_BAD_REQ;

	WCHAR ltm[1024];
	struct tm timeinfo;
	time(&client->EventTime);
	localtime_s(&timeinfo, &client->EventTime);
	_wasctime_s(ltm, 1024, &timeinfo);

	//	ucout << message.to_string() << std::endl;
	string_t x = message.request_uri().to_string();


	client->bear_print((WCHAR*)L"POST  - %ws\n", x.c_str());

	//	message.reply(status_codes::OK);
	try
	{

		client->EventCmd = L"Unknown-" + x;
		client->ListenState |= BEAR_LISTEN_BAD_REQ;

	}
	catch (...)
	{
		client->ListenState |= BEAR_LISTEN_RCV_EXP;
	}



};

//
// A DELETE .
// 
void BEARSERVER::handle_delete(http_request message)
{
	client->bear_print((WCHAR*)L"\nDELETE  - %ws\n", message.to_string().c_str());
	message.reply(status_codes::OK);
}


//
// A PUT.
// 
void BEARSERVER::handle_put(http_request message)
{
	client->bear_print((WCHAR*)L"\nPUT  - %ws\n", message.to_string().c_str());;
	message.reply(status_codes::OK);
}




int
StartListener(BearLDC *client)
{

	client->bear_print((WCHAR*)L"\n**** Start Local Listener on url - %ws ***", client->MyUrl.c_str());
	client->ListenState = BEAR_LISTEN_OFF;
	on_initialize(client->MyUrl, client);

	if (client->ListenState != BEAR_LISTEN_OK)
	{
		return -1;
	}

	client->ListenState = BEAR_LISTEN_OK;

	return 0;
}

int
StopListener(BearLDC *client)
{
	if ((client->ListenState & BEAR_LISTEN_OFF) != BEAR_LISTEN_OFF)
	{
		on_shutdown();
		client->ListenState = BEAR_LISTEN_OFF;
	}

	return 0;
}

