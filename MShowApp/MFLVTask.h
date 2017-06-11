#pragma once
#include "MShowCommon.h"
#include "MPacketQueue.h"
#include "FLVReader.h"
#include "MAudioTask.h"
#include "MAudioRenderTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"
using namespace Decode;

namespace MShow
{
	class MAudioTask;
	class MAudioRenderTask;
	class MVideoTask;
	class MVideoRenderTask;
	/// <summary>
	/// RTMP�����߳�
	/// </summary>
	class MFLVTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MFLVTask)
	public:
		MFLVTask();
		virtual ~MFLVTask();
		BOOL Initialize(HWND hWND,LPCSTR pzURL);
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	public:
		MPacketQueue&	GetAudioQueue();
		MPacketQueue&	GetVideoQueue();
		H264Decode*		GetH264();
		AACDecode*		GetAAC();
		const FLV_SCRIPTDATA& GetScript() const;
	private:
		void OnMessagePump();
	private:
		BOOL							m_bFI;
		LONGLONG						m_sample;
		TinyEvent						m_close;
		FLVReader						m_reader;
		MPacketQueue					m_audioQueue;
		MPacketQueue					m_videoQueue;
		MClock							m_clock;
		FLV_SCRIPTDATA					m_script;
		TinyScopedPtr<H264Decode>		m_h264;
		TinyScopedPtr<AACDecode>		m_aac;
		TinyScopedPtr<MVideoTask>		m_videoTask;
		TinyScopedPtr<MAudioTask>		m_audioTask;
		TinyScopedPtr<MVideoRenderTask>	m_videoRenderTask;
		TinyScopedPtr<MAudioRenderTask>	m_audioRenderTask;
	};
}



