#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <tlhelp32.h>
#include "aes.h"
#include "P751_api.h"
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/http_client.h"
#include "cpprest/filestream.h"

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace std;

using namespace http::experimental::listener;