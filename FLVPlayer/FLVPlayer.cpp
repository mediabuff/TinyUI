// TinyApp.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "FLVPlayer.h"
#include "FLVFrameUI.h"
#include "Network/TinyHTTPClient.h"
using namespace FLVPlayer;

class ClassC
{

};

class ClassB
{

};

class ClassA
{
public:
	template<typename T>
	std::vector<T>  read(ClassC const & info);
};

template<typename T>
std::vector<T>  ClassA::read(ClassC const & info)
{
	return std::vector<T>();
}

BOOL LoadSeDebugPrivilege()
{
	DWORD   err;
	HANDLE  hToken;
	LUID    Val;
	TOKEN_PRIVILEGES tp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		err = GetLastError();
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Val))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = Val;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		err = GetLastError();
		CloseHandle(hToken);
		return FALSE;
	}
	CloseHandle(hToken);
	return TRUE;
}

INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	ClassC c;
	ClassA a;
	std::vector<ClassB> ccc = a.read<ClassB>(c);

	HRESULT hRes = OleInitialize(NULL);

	LoadSeDebugPrivilege();

	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_FLVPLAYER));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	FLVFrameUI uiImpl;
	uiImpl.Create(NULL, 50, 50, 800, 800);
	uiImpl.ShowWindow(nCmdShow);
	uiImpl.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();

	OleUninitialize();
	WSACleanup();
	return loopRes;
};