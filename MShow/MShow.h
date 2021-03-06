#pragma once
#include "resource.h"
#include "MShowWindow.h"
#include "MShowController.h"
#include "Windowless/TinyVisualDocument.h"
using namespace TinyUI;

namespace MShow
{
	class MShowApp
	{
	public:
		MShowApp();
		~MShowApp();
	public:
		BOOL				Initialize(HINSTANCE hInstance, LPTSTR  lpCmdLine, INT nCmdShow, LPCTSTR lpTableName);
		BOOL				Uninitialize();
		MShowWindow&		GetView();
		MShowController&	GetController();
		INT					Run();
		QWORD				GetQPCTimeNS();
		QWORD				GetQPCTimeMS();
		QWORD				GetQPCTime100NS();
		BOOL				SleepNS(QWORD qwNSTime);
		void				SetCurrentAudioTS(LONGLONG ts);
		LONGLONG			GetCurrentAudioTS();
		string				GetDefaultPath();
	public:
		static MShowApp&	GetInstance() throw();
	private:
		string				m_szPath;
		TinyLock			m_lock;
		MShowWindow			m_window;
		MShowController		m_controller;
		TinyMessageLoop		m_msgLoop;
		LONGLONG			m_audioTS;
	};
}

