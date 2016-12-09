#pragma once
#include "../Common/TinyCore.h"
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyTaskBase.h"
#include "TinyWASAPIAudio.h"

namespace TinyUI
{
	namespace Media
	{
#define MILLISECONDS_TO_VISUALIZE 20
		/// <summary>
		/// 播放音频
		/// (eRender:音频播放设备,eCapture:音频采集设备)
		/// (eConsole:与计算机交互,eCommunications:与他人的声音交流,eMultimedia:播放或者录制电影)
		/// </summary>
		class TinyWASAPIAudioRender : public TinyWASAPIAudio, public AudioObserver
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWASAPIAudioRender)
		public:
			TinyWASAPIAudioRender();
			virtual ~TinyWASAPIAudioRender();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
		public:
			virtual BOOL	Open(const Name& name, WAVEFORMATEX* pFMT = NULL);
			virtual BOOL	Start();
			virtual BOOL	Stop();
			virtual BOOL	Reset();
			virtual BOOL	Close();
			virtual BOOL	SetVolume(FLOAT volume);
			virtual BOOL	GetVolume(FLOAT* volume);
			virtual BOOL	SetMute(BOOL bMute);
			virtual BOOL	GetMute(BOOL* bMute);
			WAVEFORMATEX*	GetInputFormat() const;
			BOOL			GetStreamLatency(REFERENCE_TIME& latency);
		private:
			void			OnMessagePump();
			BOOL			OpenClient(TinyComPtr<IMMDevice>& mmDevice, WAVEFORMATEX* pFMT);
		private:
			DWORD								m_dwChannelMask;
			UINT32								m_count;
			TinyEvent							m_sampleReady;
			TinyEvent							m_audioStop;
			IO::TinyTaskBase					m_task;
			TinyScopedArray<BYTE>				m_waveFMT;
			TinyComPtr<IAudioClient>			m_audioClient;
			TinyComPtr<IAudioClient>			m_audioClientLB;
			TinyComPtr<IAudioRenderClient>		m_audioRender;
			TinyComPtr<ISimpleAudioVolume>		m_audioVolume;
		};
	}
}
