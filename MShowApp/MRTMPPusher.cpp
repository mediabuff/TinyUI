#include "stdafx.h"
#include "MRTMPPusher.h"
#include "MShowApp.h"
#include "MShowController.h"

namespace MShow
{
#define MAX_PUSH_QUEUE_SIZE (1024 * 1024)

	MRTMPPusher::MRTMPPusher()
		:m_bClose(FALSE),
		m_dwReconnect(0)
	{
	}

	MRTMPPusher::~MRTMPPusher()
	{
	}

	BOOL MRTMPPusher::Connect(const TinyString& szURL, DWORD dwRate)
	{
		m_dwRate = dwRate;
		if (m_client.Connect(szURL))
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL MRTMPPusher::Disconnect()
	{
		return m_client.Disconnect();
	}

	BOOL MRTMPPusher::Submit()
	{
		m_bClose = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MRTMPPusher::OnMessagePump, this));
	}

	BOOL MRTMPPusher::Close(DWORD dwMS)
	{
		m_bClose = TRUE;
		m_dwReconnect = 0;
		if (TinyTaskBase::Close(dwMS))
		{
			m_client.Disconnect();
			return TRUE;
		}
		return FALSE;
	}

	void MRTMPPusher::OnMessagePump()
	{
		for (;;)
		{
			if (m_bClose)
				break;
			if (!m_samples.empty())
			{
				Sample sample;
				if (m_samples.try_pop(sample))
				{
					Publish(sample);
					SAFE_DELETE_ARRAY(sample.bits);
				}
			}
		}
	}

	void MRTMPPusher::Publish(Sample& sample)
	{
		switch (sample.mediaTag.dwType)
		{
		case 0://Video
		{
			if (sample.mediaTag.INC == 1)
			{
				MVideoEncodeTask& video = MShowApp::Instance().GetController().GetVideoEncoder();
				MAudioEncodeTask& audio = MShowApp::Instance().GetController().GetAudioEncoder();
				TinySize pulgSize = video.GetSize();
				m_client.SendMetadata(pulgSize.cx, pulgSize.cy, video.GetVideoFPS(), video.GetVideoRate(), audio.GetFormat(), audio.GetAudioRate());
				video.GetQSV().GetSPSPPS(m_latestSPS, m_latestPPS);
				m_client.SendSPP(m_latestPPS, m_latestSPS, sample.mediaTag.dwTime);
			}
			if (sample.mediaTag.dwFlag & MFX_FRAMETYPE_I ||
				sample.mediaTag.dwFlag & MFX_FRAMETYPE_IDR ||
				sample.mediaTag.dwFlag & MFX_FRAMETYPE_P ||
				sample.mediaTag.dwFlag & MFX_FRAMETYPE_B ||
				sample.mediaTag.dwFlag & MFX_FRAMETYPE_S ||
				sample.mediaTag.dwFlag & MFX_FRAMETYPE_REF)
			{
				m_client.SendVideo(sample.bits, sample.size, sample.mediaTag.dwTime);
			}
		}
		break;
		case 1://Audio
		{
			if (sample.mediaTag.INC == 1)
			{
				MAudioEncodeTask& encoder = MShowApp::Instance().GetController().GetAudioEncoder();
				vector<BYTE> info;
				encoder.GetAAC().GetSpecificInfo(info);
				m_client.SendAAC(&info[0], info.size());
			}
			m_client.SendAudio(sample.bits, sample.size, sample.mediaTag.dwTime);
		}
		break;
		}
		SAFE_DELETE_ARRAY(sample.bits);
	}
}
