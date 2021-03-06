#include "stdafx.h"
#include "FLVDecode.h"
#include "Network/TinyHTTPRequest.h"
#include "Network/TinyHTTPResponse.h"
using namespace TinyUI::Network;

namespace FLVPlayer
{
	FLVDecode::FLVDecode(HWND hWND)
		:m_hWND(hWND),
		m_audioQueue(m_lock1),
		m_videoQueue(m_lock2),
		m_videoTask(*this),
		m_audioTask(*this),
		m_audioRender(m_audioTask),
		m_videoRender(m_videoTask),
		m_baseTime(-1),
		m_basePTS(-1),
		m_bFirstI(FALSE)
	{
		m_aac.Reset(new AACDecode());
		m_x264.Reset(new x264Decode());
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}

	FLVDecode::~FLVDecode()
	{

	}
	BOOL FLVDecode::Submit()
	{
		//if (m_reader.OpenURL("rtmp://live.hkstv.hk.lxdns.com/live/hks"))
		if(m_reader.OpenURL("http://10.110.48.109:31152/6696864323&type=http&flashId=flv"))
		//if (m_reader.OpenURL("http://10.110.48.31:16654/73278a4a86960eeb576a8fd4c9ec6997"))
		{
			m_size.cx = static_cast<LONG>(m_reader.GetScript().width);
			m_size.cy = static_cast<LONG>(m_reader.GetScript().height);
			if (TinyTaskBase::Submit(BindCallback(&FLVDecode::OnMessagePump, this)))
			{
				m_audioRender.Submit();
				m_audioTask.Submit();
				m_videoRender.Submit();
				m_videoTask.Submit();
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL FLVDecode::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		m_audioRender.Close(dwMS);
		m_videoRender.Close(dwMS);
		m_videoTask.Close(dwMS);
		m_audioTask.Close(dwMS);
		return TinyTaskBase::Close(dwMS);
	}

	void FLVDecode::OnMessagePump()
	{
		SampleTag tag = { 0 };
		FLV_BLOCK block = { 0 };
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_audioQueue.GetSize() + m_videoQueue.GetSize();
			if (size > MAX_QUEUE_SIZE)
			{
				Sleep(5);
				continue;
			}
			if (!m_reader.ReadBlock(block))
			{
				break;
			}
			if (block.type == FLV_AUDIO)
			{
				if (block.audio.codeID == FLV_CODECID_AAC)
				{
					if (block.audio.packetType == FLV_AudioSpecificConfig)
					{
						if (!m_aac->Open(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16))
						{
							goto _ERROR;
						}
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						if (!m_bFirstI)
						{
							SAFE_DELETE_ARRAY(block.audio.data);
							SAFE_DELETE_ARRAY(block.video.data);
							continue;
						}
						ZeroMemory(&tag, sizeof(tag));
						tag.size = block.video.size;
						tag.bits = new BYTE[tag.size];
						memcpy_s(tag.bits, tag.size, block.audio.data, block.audio.size);
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_audioQueue.Push(tag);
					}
				}
			}
			if (block.type == FLV_VIDEO)
			{
				if (block.video.codeID == FLV_CODECID_H264)
				{
					if (block.video.packetType == FLV_AVCDecoderConfigurationRecord)
					{
						//if (!m_qsv.Open(block.video.data, block.video.size))
						//{
						//	goto _ERROR;
						//}
						if (!m_x264->Initialize(m_size, m_size))
						{
							goto _ERROR;
						}
						if (!m_x264->Open(block.video.data, block.video.size))
						{
							goto _ERROR;
						}
						SAFE_DELETE_ARRAY(block.audio.data);
						SAFE_DELETE_ARRAY(block.video.data);
					}
					if (block.video.packetType == FLV_NALU)
					{
						if (!m_bFirstI)
						{
							if (block.video.codeType != 1)
							{
								SAFE_DELETE_ARRAY(block.audio.data);
								SAFE_DELETE_ARRAY(block.video.data);
								continue;
							}
							m_bFirstI = TRUE;
						}
						ZeroMemory(&tag, sizeof(tag));
						tag.size = block.video.size;
						tag.bits = new BYTE[tag.size];
						memcpy_s(tag.bits, tag.size, block.video.data, block.video.size);
						tag.sampleDTS = block.dts;
						tag.samplePTS = block.pts;
						m_videoQueue.Push(tag);
					}
				}
			}
		}
	_ERROR:
		SAFE_DELETE_ARRAY(block.audio.data);
		SAFE_DELETE_ARRAY(block.video.data);
	}

	//////////////////////////////////////////////////////////////////////////
	FLVAudioRender::FLVAudioRender(FLVVAudioTask& decode)
		:m_decode(decode),
		m_bInitialize(FALSE),
		m_bFlag(FALSE),
		m_dwMS(0)
	{
		m_close.CreateEvent();
		m_events[0].CreateEvent(TRUE, FALSE);
		m_events[1].CreateEvent(TRUE, FALSE);
		m_events[2].CreateEvent(TRUE, FALSE);
	}
	FLVAudioRender::~FLVAudioRender()
	{

	}
	BOOL FLVAudioRender::Submit()
	{
		m_player.Close();
		if (!m_player.Open(m_decode.m_decode.m_hWND))
			return FALSE;
		return TinyTaskBase::Submit(BindCallback(&FLVAudioRender::OnMessagePump, this));
	}
	BOOL FLVAudioRender::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	void FLVAudioRender::OnMessagePump()
	{
		TinyTimer timer;
		DWORD	dwOffset = 0;
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			SampleTag tag = m_decode.m_queue.Pop();
			if (tag.size <= 0)
				continue;
			if (tag.samplePTS == m_decode.m_decode.m_basePTS)
			{
				TinyAutoLock lock(m_decode.m_decode.m_lockTime);
				m_decode.m_decode.m_baseTime = timeGetTime();
			}
			while (m_decode.m_decode.m_baseTime == -1);
			if (!m_bInitialize)
			{
				m_timer.BeginTime();
				m_bInitialize = TRUE;
				if (!m_player.SetFormat(m_decode.m_decode.m_aac->GetFormat(), tag.size * 3))
					break;
				DSBPOSITIONNOTIFY vals[3];
				vals[0].dwOffset = tag.size - 1;
				vals[0].hEventNotify = m_events[0];
				vals[1].dwOffset = tag.size * 2 - 1;
				vals[1].hEventNotify = m_events[1];
				vals[2].dwOffset = tag.size * 3 - 1;
				vals[2].hEventNotify = m_events[2];
				m_player.SetNotifys(3, vals);
				m_timer.EndTime();
				m_decode.m_decode.m_lockTime.Lock();
				m_decode.m_decode.m_baseTime += m_timer.GetMillisconds();
				m_decode.m_decode.m_lockTime.Unlock();
				m_player.Play();
				DWORD dwMS = timeGetTime() - m_decode.m_decode.m_baseTime;
				INT offset = tag.samplePTS - dwMS;
				if (timer.Wait(offset, 1000))
				{
					if (tag.size != 4096)
					{
						m_player.Fill(tag.bits, tag.size, dwOffset);
					}
				}
				//Sleep(offset < 0 ? 0 : offset);
			}
			else
			{
				m_player.Fill(tag.bits, tag.size, dwOffset);
				SAFE_DELETE_ARRAY(tag.bits);
			}
			HANDLE handles[3] = { m_events[0],m_events[1],m_events[2] };
			HRESULT hRes = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
			switch (hRes)
			{
			case WAIT_OBJECT_0:
				dwOffset = 0;
				break;
			case WAIT_OBJECT_0 + 1:
				dwOffset = m_player.GetSize() / 3 * 1;
				break;
			case WAIT_OBJECT_0 + 2:
				dwOffset = m_player.GetSize() / 3 * 2;
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVideoRender::FLVVideoRender(FLVVideoTask& decode)
		:m_decode(decode),
		m_wPTS(0),
		m_bFlag(FALSE),
		m_dwMS(0)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVideoRender::~FLVVideoRender()
	{

	}
	BOOL FLVVideoRender::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVVideoRender::OnMessagePump, this));
	}
	BOOL FLVVideoRender::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	void FLVVideoRender::OnMessagePump()
	{
		TinyTimer timer;
		for (;;)
		{
			if (m_close.Lock(0))
				break;
			SampleTag tag = m_decode.m_queue.Pop();
			if (tag.size <= 0)
				continue;
			if (tag.samplePTS == m_decode.m_decode.m_basePTS)
			{
				TinyAutoLock lock(m_decode.m_decode.m_lockTime);
				m_decode.m_decode.m_baseTime = timeGetTime();
			}
			while (m_decode.m_decode.m_baseTime == -1);
			DWORD dwMS = timeGetTime() - m_decode.m_decode.m_baseTime;
			INT offset = tag.samplePTS - dwMS;
			if (timer.Wait(offset, 1000))
			{
				OnRender(tag.bits, tag.size);
			}
			//Sleep(offset < 0 ? 0 : offset);
			SAFE_DELETE_ARRAY(tag.bits);
		}
	}
	void FLVVideoRender::OnRender(BYTE* bits, LONG size)
	{
		ASSERT(size == m_decode.m_decode.m_size.cx *  m_decode.m_decode.m_size.cy * 4);
		HDC hDC = GetDC(m_decode.m_decode.m_hWND);
		if (hDC != NULL)
		{
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_decode.m_decode.m_size.cx;
			bmi.bmiHeader.biHeight = -m_decode.m_decode.m_size.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = m_decode.m_decode.m_size.cx *  m_decode.m_decode.m_size.cy * 4;
			BYTE* pvBits = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pvBits), NULL, 0);
			if (hBitmap != NULL)
			{
				memcpy(pvBits, bits, size);
				TinyMemDC dc(hDC, hBitmap);
				TinyRectangle src = { 0,0, m_decode.m_decode.m_size.cx, m_decode.m_decode.m_size.cy };
				dc.Render(src, src, FALSE);
				SAFE_DELETE_OBJECT(hBitmap);
			}
			ReleaseDC(m_decode.m_decode.m_hWND, hDC);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVideoTask::FLVVideoTask(FLVDecode& decode)
		:m_decode(decode),
		m_queue(m_lock)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVideoTask::~FLVVideoTask()
	{

	}
	BOOL FLVVideoTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVVideoTask::OnMessagePump, this));
	}
	BOOL FLVVideoTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}

	void FLVVideoTask::OnQSV(Media::SampleTag& tag)
	{
		if (m_decode.m_basePTS == -1)
		{
			m_decode.m_basePTS = tag.samplePTS;
		}
		m_queue.Push(tag);
	}

	void FLVVideoTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_queue.GetSize();
			if (size > MAX_VIDEO_QUEUE_SIZE)
			{
				Sleep(3);
				continue;
			}
			SampleTag tag = m_decode.m_videoQueue.Pop();
			if (tag.size > 0)
			{
				//mfxFrameSurface1* surface1 = NULL;
				//if (m_decode.m_qsv.Decode(tag, surface1))
				//{
				//	QSV::QSVAllocator* pAllocator = m_decode.m_qsv.GetAllocator();
				//	pAllocator->Lock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				//	tag.size = surface1->Info.CropH * surface1->Data.Pitch;
				//	tag.bits = new BYTE[tag.size];
				//	memcpy(tag.bits, surface1->Data.B, tag.size);
				//	pAllocator->Unlock(pAllocator->pthis, surface1->Data.MemId, &(surface1->Data));
				//	m_decode.m_qsv.UnlockSurface(surface1);
				//	if (m_decode.m_basePTS == -1)
				//	{
				//		m_decode.m_basePTS = tag.samplePTS;
				//	}
				//	m_queue.Push(tag);
				//}
				BYTE* bo = NULL;
				LONG  so = 0;
				if (m_decode.m_x264->Decode(tag, bo, so))
				{
					SAFE_DELETE_ARRAY(tag.bits);
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					tag.size = so;
					tag.samplePTS = m_decode.m_x264->GetYUV420()->pts;
					tag.sampleDTS = tag.samplePTS;
					if (m_decode.m_basePTS == -1)
					{
						m_decode.m_basePTS = tag.samplePTS;
					}
					m_queue.Push(tag);
				}
				else
				{
					SAFE_DELETE_ARRAY(tag.bits);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FLVVAudioTask::FLVVAudioTask(FLVDecode& decode)
		:m_decode(decode),
		m_queue(m_lock)
	{
		m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
	}
	FLVVAudioTask::~FLVVAudioTask()
	{

	}
	BOOL FLVVAudioTask::Submit()
	{
		return TinyTaskBase::Submit(BindCallback(&FLVVAudioTask::OnMessagePump, this));
	}
	BOOL FLVVAudioTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyTaskBase::Close(dwMS);
	}
	void FLVVAudioTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			INT size = m_queue.GetSize();
			if (size > MAX_AUDIO_QUEUE_SIZE || m_decode.m_audioQueue.IsEmpty())
			{
				Sleep(3);
				continue;
			}
			SampleTag tag = m_decode.m_audioQueue.Pop();
			if (tag.size > 0)
			{
				BYTE* bo = NULL;
				LONG  so = 0;
				if (m_decode.m_aac->Decode(tag, bo, so))
				{
					tag.bits = new BYTE[so];
					memcpy(tag.bits, bo, so);
					tag.size = so;
					if (m_decode.m_basePTS == -1)
					{
						m_decode.m_basePTS = tag.samplePTS;
					}
					m_queue.Push(tag);
				}
			}
		}
	}
}

