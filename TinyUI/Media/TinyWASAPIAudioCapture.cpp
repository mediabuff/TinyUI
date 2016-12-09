#include "../stdafx.h"
#include "TinyWASAPIAudioCapture.h"
#include <algorithm>

namespace TinyUI
{
	namespace Media
	{
		TinyWASAPIAudioCapture::TinyWASAPIAudioCapture()
		{
			m_sampleReady.CreateEvent(FALSE, FALSE, NULL, NULL);
			m_audioStop.CreateEvent(FALSE, FALSE, NULL, NULL);
		}

		TinyWASAPIAudioCapture::~TinyWASAPIAudioCapture()
		{
			Close();
		}
		BOOL TinyWASAPIAudioCapture::Open(const Name& name, WAVEFORMATEX* pFMT)
		{
			TinyComPtr<IMMDeviceEnumerator> enumerator;
			HRESULT hRes = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMMDevice> mmDevice;
			wstring ws = StringToWString(name.id());
			hRes = enumerator->GetDevice(ws.c_str(), &mmDevice);
			if (FAILED(hRes))
				return FALSE;
			return OpenClient(mmDevice, pFMT);
		}
		BOOL TinyWASAPIAudioCapture::OpenClient(TinyComPtr<IMMDevice>& mmDevice, WAVEFORMATEX* pFMT)
		{
			ASSERT(mmDevice);
			BOOL	bFlag = (pFMT == NULL);
			HRESULT hRes = S_OK;
			DWORD state = DEVICE_STATE_DISABLED;
			hRes = mmDevice->GetState(&state);
			if (FAILED(hRes))
				goto MMERROR;
			if (!(state & DEVICE_STATE_ACTIVE))
				return FALSE;
			hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClient);
			if (FAILED(hRes))
				goto MMERROR;
			if (pFMT == NULL)
			{
				hRes = m_audioClient->GetMixFormat(&pFMT);
				if (hRes != S_OK)
					goto MMERROR;
			}
			hRes = m_audioClient->GetMixFormat(&pFMT);
			if (hRes != S_OK)
				goto MMERROR;
			if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
			{
				//事件模式
				hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwStreamFlag, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, pFMT, NULL);
				if (hRes != S_OK)
					goto MMERROR;
			}
			else
			{
				//主动模式
				hRes = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, m_dwStreamFlag, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, pFMT, NULL);
				if (hRes != S_OK)
					goto MMERROR;
			}
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX) + pFMT->cbSize]);
			memcpy(m_waveFMT, (BYTE*)pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			//一般都是960
			hRes = m_audioClient->GetBufferSize(&m_count);
			if (FAILED(hRes))
				goto MMERROR;
			REFERENCE_TIME defaultPeriod = 0;
			REFERENCE_TIME minimumDevicePeriod = 0;
			hRes = m_audioClient->GetDevicePeriod(&defaultPeriod, &minimumDevicePeriod);
			if (FAILED(hRes))
				goto MMERROR;
			if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_LOOPBACK)
			{
				hRes = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)&m_audioClientLB);
				if (hRes != S_OK)
					goto MMERROR;
				hRes = m_audioClientLB->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, MILLISECONDS_TO_VISUALIZE * MFTIMES_PER_MS, 0, pFMT, NULL);
				if (hRes != S_OK)
					goto MMERROR;
				hRes = m_audioClientLB->SetEventHandle(m_sampleReady);
				if (hRes != S_OK)
					goto MMERROR;
			}
			if (m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
			{
				hRes = m_audioClient->SetEventHandle(m_sampleReady);
				if (hRes != S_OK)
					goto MMERROR;
			}
			hRes = m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_audioCapture);
			if (hRes != S_OK)
				goto MMERROR;
			hRes = m_audioClient->GetService(__uuidof(ISimpleAudioVolume), (void**)&m_audioVolume);
			if (hRes != S_OK)
				goto MMERROR;
			return TRUE;
		MMERROR:
			if (pFMT && bFlag)
			{
				CoTaskMemFree(pFMT);
				pFMT = NULL;
			}
			return FALSE;
		}
		BOOL TinyWASAPIAudioCapture::Start()
		{
			ASSERT(m_audioClient);
			m_audioStop.ResetEvent();
			m_task.Close(INFINITE);
			if (!m_task.Submit(BindCallback(&TinyWASAPIAudioCapture::OnMessagePump, this)))
				return FALSE;
			HRESULT hRes = m_audioClient->Start();
			if (FAILED(hRes))
				return FALSE;
			if (m_audioClientLB)
			{
				hRes = m_audioClientLB->Start();
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::Stop()
		{
			m_audioStop.SetEvent();
			m_task.Close(INFINITE);
			if (m_audioClient)
			{
				HRESULT hRes = m_audioClient->Stop();
				if (hRes != S_OK)
					return FALSE;
			}
			m_audioVolume.Release();
			m_audioCapture.Release();
			m_audioClient.Release();
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::Reset()
		{
			HRESULT hRes = m_audioClient->Reset();
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}

		BOOL TinyWASAPIAudioCapture::SetVolume(FLOAT volume)
		{
			ASSERT(m_audioVolume);
			return m_audioVolume->SetMasterVolume(volume, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetVolume(FLOAT* volume)
		{
			ASSERT(m_audioVolume);
			return m_audioVolume->GetMasterVolume(volume) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::SetMute(BOOL bMute)
		{
			ASSERT(m_audioVolume);
			return m_audioVolume->SetMute(bMute, NULL) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetMute(BOOL* bMute)
		{
			ASSERT(m_audioVolume);
			return m_audioVolume->GetMute(bMute) == S_OK;
		}

		BOOL TinyWASAPIAudioCapture::GetStreamLatency(REFERENCE_TIME& latency)
		{
			ASSERT(m_audioClient);
			return m_audioClient->GetStreamLatency(&latency) == S_OK;
		}
		WAVEFORMATEX* TinyWASAPIAudioCapture::GetInputFormat() const
		{
			if (m_waveFMT)
			{
				return reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
			}
			return NULL;
		}
		BOOL TinyWASAPIAudioCapture::Close()
		{
			if (Stop() && m_audioClientLB)
			{
				return SUCCEEDED(m_audioClientLB->Stop());
			}
			return TRUE;
		}
		void TinyWASAPIAudioCapture::OnMessagePump()
		{
			WAVEFORMATEX* pFMT = GetInputFormat();
			TinyScopedAvrt avrt("Pro Audio");
			avrt.SetPriority();
			HANDLE waits[2] = { m_audioStop ,m_sampleReady };
			BOOL bBreak = FALSE;
			while (!bBreak)
			{
				DWORD dwMS = m_dwStreamFlag & AUDCLNT_STREAMFLAGS_EVENTCALLBACK ? INFINITE : MILLISECONDS_TO_VISUALIZE / 2;
				DWORD dwRes = WaitForMultipleObjects(2, waits, FALSE, dwMS);
				switch (dwRes)
				{
				case WAIT_OBJECT_0 + 0:
				case WAIT_FAILED:
				case WAIT_ABANDONED:
					bBreak = TRUE;
					break;
				case WAIT_TIMEOUT:
				case WAIT_OBJECT_0 + 1:
				{
					DWORD	dwFlags = 0;
					UINT32	count = 0;
					BYTE*	bits = NULL;
					if (SUCCEEDED(m_audioCapture->GetBuffer(&bits, &count, &dwFlags, NULL, NULL)))
					{
						if (dwFlags & AUDCLNT_BUFFERFLAGS_SILENT)
						{
							bits = NULL;
						}
						OnDataAvailable(bits, count, this);
						m_audioCapture->ReleaseBuffer(count);
					}
				}
				break;
				}
			}
		}

		void TinyWASAPIAudioCapture::OnDataAvailable(BYTE* bits, LONG count, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, count, lpParameter);
			}
		}
	}
}
