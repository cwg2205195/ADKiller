// pch.cpp: 与预编译标头对应的源文件；编译成功所必需的

#include "pch.h"/*
STARTUPINFOA si;
PROCESS_INFORMATION pi;*/

DllInjector::DllInjector()
{
	success = false;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(CmdLine, 0, sizeof(CmdLine));
	memset(ExeName, 0, sizeof(ExeName));
}
void DllInjector::NewProcess(const wchar_t * szExeName,const wchar_t *cmd)
{
	if (szExeName == NULL && cmd == NULL)
		return;
	wchar_t * p1 = 0, *p2 = 0;

	if (szExeName)
	{
		//ExeName = (wchar_t*)malloc(MAX_PATH);
		//memset(ExeName, 0, MAX_PATH);
		memcpy(ExeName, szExeName, wcslen(szExeName) * 2 + 2);
		p1 = ExeName;
	}
	if (cmd)
	{
		//CmdLine = (wchar_t*)malloc(MAX_PATH);
		//memset(CmdLine, 0, MAX_PATH);
		memcpy(CmdLine, cmd, wcslen(cmd) * 2 + 2);
		p2 = CmdLine;
	}
	BOOL ret = CreateProcessW(
		(LPWSTR)p1,
		(LPWSTR)p2,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS, //CREATE_SUSPENDED NORMAL_PRIORITY_CLASS
		NULL,
		NULL,
		&this->si,
		&this->pi
	);
	if (ret == NULL)
	{
		printf("Can not createprocess: %08X \n", GetLastError());
		return;
	}
	success = true;
}

void DllInjector::InjectDll(const char *szDllName)
{
	if (szDllName == NULL || !success)
		return;

	//获取LoadLibrary的地址
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	PROC proc = GetProcAddress(hKernel32, "LoadLibraryA");
	LPVOID hMem = VirtualAllocEx(pi.hProcess, NULL, sizeof(dllName) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (GetLastError() != ERROR_SUCCESS)
		return;
	DWORD bWrite;
	WriteProcessMemory(pi.hProcess, hMem, szDllName, strlen(szDllName)+1, &bWrite);	
	DWORD tid;
	HANDLE hThread = CreateRemoteThread(pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)proc, hMem, NULL, &tid);
	if (hThread == NULL)
		return;
	CloseHandle(hThread);
	Sleep(1 * 1000);
}

DllInjector::~DllInjector()
{
	if (success)
	{
		//释放资源
		CloseHandle(this->pi.hThread);
		CloseHandle(this->pi.hProcess);
	}
}

//启动目标进程，成功休眠3秒等待初始化，然后返回True
/*/bool StartProc(void)
{
	BOOL ret = CreateProcessA(
		NULL,
		(LPSTR)procName,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED, //CREATE_SUSPENDED NORMAL_PRIORITY_CLASS
		NULL,
		NULL,
		&si,
		&pi
	);
	if (ret == NULL)
	{
		printf("Can not createprocess: %08X \n", GetLastError());
		return false;
	}
	//Sleep(3 * 1000);
	return ret;
}

//把DLL注入到目标进程
void InjectDll()
{
	//获取LoadLibrary的地址
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	PROC proc = GetProcAddress(hKernel32, "LoadLibraryA");
	LPVOID hMem = VirtualAllocEx(pi.hProcess, NULL, sizeof(dllName) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (GetLastError() != ERROR_SUCCESS)
		return;
	DWORD bWrite;
	WriteProcessMemory(pi.hProcess, hMem, dllName, sizeof(dllName), &bWrite);	//不知道会不会写0 
	DWORD tid;
	HANDLE hThread = CreateRemoteThread(pi.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)proc, hMem, NULL, &tid);
	if (hThread == NULL)
		return;
	CloseHandle(hThread);
	Sleep(1 * 1000);
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}*/

// 一般情况下，忽略此文件，但如果你使用的是预编译标头，请保留它。
