// GameDetour.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "GameDetour.h"
#include "DX9Capture.h"
#include "DXGICapture.h"
#include "OpenGLCapture.h"
using namespace DXCapture;

namespace GameDetour
{
	GameCapture::GameCapture()
		:m_hTask(NULL),
		m_hWNDD3D(NULL),
		m_bDX9Detour(FALSE),
		m_bDXGIDetour(FALSE)
	{

	}
	GameCapture::~GameCapture()
	{

	}
	BOOL GameCapture::Attach(HMODULE hModule)
	{
		m_hInstance = hModule;
		m_hTask = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GameCapture::CaptureTask, (LPVOID)this, 0, 0);
		return m_hTask != NULL;
	}
	void GameCapture::Detach()
	{
		m_bDX9Detour = m_bDXGIDetour = FALSE;
		if (m_hTask != NULL)
		{
			if (m_hWNDD3D != NULL)
			{
				::DestroyWindow(m_hWNDD3D);
				m_hWNDD3D = NULL;
			}
			PostQuitMessage(0);
			WaitForSingleObject(m_hTask, INFINITE);
			CloseHandle(m_hTask);
			m_hTask = NULL;
		}
	}
	BOOL GameCapture::TryCapture()
	{
		if (!m_hWNDD3D)
		{
			return FALSE;
		}
		if (!m_bDX9Detour)
		{
			m_bDX9Detour = DX9Capture::Instance().Initialize(m_hWNDD3D);
		}
		return m_bDX9Detour;
		/*	if (!m_bDXGIDetour)
			{
				m_bDXGIDetour = DXGICapture::Instance().Initialize(m_hWNDD3D);
			}
			return m_bDX9Detour || m_bDXGIDetour;*/
	}
	DWORD WINAPI GameCapture::CaptureTask(LPVOID ps)
	{
		GameCapture* _this = reinterpret_cast<GameCapture*>(ps);
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_OWNDC;
		wc.hInstance = _this->m_hInstance;
		wc.lpfnWndProc = (WNDPROC)DefWindowProc;
		wc.lpszClassName = D3D_WINDOWCLASS;
		if (RegisterClass(&wc))
		{
			_this->m_hWNDD3D = CreateWindowEx(0,
				D3D_WINDOWCLASS,
				TEXT("D3D Caption Window"),
				WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				0, 0,
				1, 1,
				NULL,
				NULL,
				_this->m_hInstance,
				NULL
			);
			if (!_this->m_hWNDD3D)
				return FALSE;
		}
		_this->TryCapture();
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return FALSE;
	}
}

