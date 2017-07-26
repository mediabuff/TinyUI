#pragma once
#include "TinyWave.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <vector>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")

namespace TinyUI
{
	namespace Media
	{
		typedef struct tagPLAYDEVICE
		{
			GUID	Guid;
			string	Description;
			string	Module;
		}PLAYDEVICE;

		class TinySoundPlayer
		{
		public:
			TinySoundPlayer();
			virtual ~TinySoundPlayer();
		public:
			BOOL	Enumerate(std::vector<PLAYDEVICE>& devices);
			BOOL	Initialize(HWND hWND);
			BOOL	SetNotifys(DWORD dwSize, LPCDSBPOSITIONNOTIFY pNotify);
			BOOL	SetFormat(WAVEFORMATEX* pFMT, DWORD dwSize);
			BOOL	Play(DWORD dwFlags = DSBPLAY_LOOPING);
			BOOL	Fill(BYTE* bits, LONG size, DWORD dwOffset);
			BOOL	GetCaps(DSCAPS& caps);
			BOOL	SetVolume(LONG volume);
			BOOL	GetVolume(LONG& volume);
			BOOL	SetFrequency(DWORD dwFrequency);
			BOOL	GetFrequency(DWORD& dwFrequency);
			BOOL	SetPlan(LONG plan);
			BOOL	GetPlan(LONG& plan);
			BOOL	Stop();
			BOOL	Close();
			BOOL	IsValid() const;
			/// <summary>
			/// �����С
			/// </summary>
			DWORD	GetSize() const;
		private:
			static BOOL CALLBACK DSEnumCallback(LPGUID pzGUID, LPCSTR pzDesc, LPCSTR pzModule, LPVOID pContext);
		private:
			TinyComPtr<IDirectSound8>		m_sound;
			TinyComPtr<IDirectSoundBuffer>	m_primaryDSB;
			TinyComPtr<IDirectSoundBuffer8>	m_secondaryDSB;
			WAVEFORMATEX*					m_waveFMT;
			DWORD							m_dwSize;
			DWORD							m_dwOffset;
		};
	}
}

