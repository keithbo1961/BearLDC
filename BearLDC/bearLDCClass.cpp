#include "BearLDC.h"

BearLDC::BearLDC()
{

	struct connect_struct *ptr;
	struct connect_ports *ptr1;
	struct connect_threads *ptr2;

	try {
		ptr = (connect_struct *)malloc(sizeof(connect_struct));
		ptr1 = (connect_ports *)malloc(sizeof(connect_ports));
		ptr2 = (connect_threads *)malloc(sizeof(connect_threads));

		if (ptr == NULL || ptr1 == NULL || ptr2 == NULL)
		{
			bear_print((TCHAR *)L"\nMalloc Error on CCL initition...\n");
		}

		ptr->ip = 0;
		ptr->next = NULL;
		ptr1->port = 0;
		ptr1->next = NULL;
		ptr2->ThreadID.p = 0;
		time(&ptr2->stime);
		ptr2->next = NULL;

		ConnectList.head = ptr;
		ConnectList.ccnt = 1;
		ConnectList.phead =  ptr1;
		ConnectList.pcnt = 1;
		ConnectList.thead = ptr2;
		ConnectList.tcnt = 1;


	}
	catch (...)
	{
		bear_print((TCHAR *)L"\nBraLDC Class initition failure...\n");
	}
}

/* Logging Utiliy functions. */
void
BearLDC::bear_print(CHAR    *fmt, ...)
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
BearLDC::bear_print(WCHAR    *fmt, ...)
{
	va_list     ap;
	WCHAR        output_buffer[1024];


	std::wofstream f(this->LogFile, ios_base::app);

	va_start(ap, fmt);
	vswprintf_s(output_buffer, 1024, fmt, ap);

	if (!LogToFile)
	{
		OutputDebugString(output_buffer);
		va_end(ap);
		return;
	}

	if (f.is_open())
	{

		f << output_buffer;
	}
	else
	{
		OutputDebugString(output_buffer);
	}

	f.close();
	va_end(ap);
}

DWORD
BearLDC::ExecProc(TCHAR *cmdline)
{
	STARTUPINFO             startup_info;
	PROCESS_INFORMATION     process_info;
	HANDLE                  hProcess;
	BOOL                    bRet;
	DWORD                   results;

	GetStartupInfo(&startup_info);
	bRet = CreateProcess(
		NULL,
		cmdline,
		NULL,
		NULL,
		FALSE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&startup_info,
		&process_info
	);


	if (!bRet)
	{
		bear_print((TCHAR *)L"\nError Creating Process - %ld", GetLastError());
		return (-1);
	}

	hProcess = process_info.hProcess;
	CloseHandle(process_info.hThread);

	WaitForSingleObject(hProcess, INFINITE);

	if (!GetExitCodeProcess(hProcess, &results))
	{

		bear_print((TCHAR *)L"\nError Retrieving Process exit code - %ld", GetLastError());
		CloseHandle(hProcess);
		return (-2);
	}

	CloseHandle(hProcess);

	return results;
}

DWORD
BearLDC::FindProcessId(const WCHAR *processname)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	DWORD result = NULL;



	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32); // <----- IMPORTANT

										  // Retrieve information about the first process,
										  // and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object
											//	printf("!!! Failed to gather information on system processes! \n");
		return(NULL);
	}

	do
	{
		//printf("Checking process %ls\n", pe32.szExeFile);
		if (0 == wcscmp(processname, pe32.szExeFile))
		{
			result = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return result;
}








