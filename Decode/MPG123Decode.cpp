#include "stdafx.h"
#include "MPG123Decode.h"

namespace Decode
{
	MPG123Decode::MPG123Decode()
		:m_handle(NULL)
	{
		mpg123_init();
	}


	MPG123Decode::~MPG123Decode()
	{
		mpg123_exit();
	}
	BOOL MPG123Decode::Open()
	{
		INT iRes = 0;
		m_handle = mpg123_new(NULL, &iRes);
		if (!m_handle)
			return FALSE;
		iRes = mpg123_param(m_handle, MPG123_VERBOSE, 2, 0);
		if (iRes == MPG123_ERR)
			goto _ERROR;
		iRes = mpg123_open_feed(m_handle);
		if (iRes == MPG123_ERR)
			goto _ERROR;
		return TRUE;
	_ERROR:
		mpg123_close(m_handle);
		mpg123_delete(m_handle);
		m_handle = NULL;
		return FALSE;
	}
	void MPG123Decode::Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback)
	{
		m_callback = std::move(callback);
	}
	BOOL MPG123Decode::Decode(BYTE* rawdata, LONG rawsize)
	{
		if (!m_handle)
			return FALSE;
		INT iRes = MPG123_OK;
		for (;;)
		{
			size_t outsize = 0;
			iRes = mpg123_decode(m_handle, rawdata, rawsize, m_raw, MPG123_OUTBUFFER_SIZE, &outsize);
			if (iRes > MPG123_OK || iRes == MPG123_ERR)
				return FALSE;
			if (iRes == MPG123_NEED_MORE)
			{
				return TRUE;
			}
			if (iRes == MPG123_OK ||
				iRes == MPG123_DONE)
			{
				if (!m_callback.IsNull())
				{
					m_callback(m_raw, outsize, reinterpret_cast<LPVOID>(m_waveFMT.Ptr()));
				}
				return TRUE;
			}
			if (iRes == MPG123_NEW_FORMAT)
			{
				INT  encoding = 0;
				INT  channels = 0;
				LONG samplesPerSec = 0;
				iRes = mpg123_getformat(m_handle, &samplesPerSec, &channels, &encoding);
				if (iRes != MPG123_OK)
					return FALSE;
				if (m_waveFMT.IsEmpty())
				{
					m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX)]);
					if (samplesPerSec == 0)
						samplesPerSec = 44100;
					if (channels == 0)
						channels = MPG123_STEREO;
					if (encoding == 0)
						encoding = MPG123_ENC_SIGNED_16;
					iRes = mpg123_format_none(m_handle);
					if (iRes != MPG123_OK)
						return FALSE;
					iRes = mpg123_format(m_handle, samplesPerSec, channels, encoding);
					if (iRes != MPG123_OK)
						return FALSE;
					WAVEFORMATEX* pFMT = reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
					pFMT->cbSize = 0;
					pFMT->wFormatTag = WAVE_FORMAT_PCM;
					pFMT->nChannels = channels;
					pFMT->nSamplesPerSec = samplesPerSec;
					pFMT->wBitsPerSample = MPG123_SAMPLESIZE(encoding) * 8;
					pFMT->nBlockAlign = pFMT->nChannels * (pFMT->wBitsPerSample / 8);
					pFMT->nAvgBytesPerSec = pFMT->nChannels * pFMT->nSamplesPerSec * pFMT->wBitsPerSample / 8;
				}
				if (!m_callback.IsNull())
				{
					m_callback(m_raw, 0, reinterpret_cast<LPVOID>(m_waveFMT.Ptr()));
				}
			}
		}
	}
	BOOL MPG123Decode::Close()
	{
		if (m_handle != NULL)
		{
			mpg123_close(m_handle);
			mpg123_delete(m_handle);
			m_handle = NULL;
		}
		return TRUE;
	}
	BOOL MPG123Decode::SetFormat(WAVEFORMATEX& sFMT)
	{
		if (m_waveFMT.IsEmpty())
		{
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX)]);
		}
		memcpy(m_waveFMT, &sFMT, sizeof(WAVEFORMATEX));
		if (m_handle != NULL)
		{
			INT iRes = mpg123_format_none(m_handle);
			if (iRes != MPG123_OK)
				return FALSE;
			INT encoding = sFMT.wBitsPerSample / 8;
			if (encoding == 1)
				encoding = MPG123_ENC_SIGNED_8;
			if (encoding == 2)
				encoding = MPG123_ENC_SIGNED_16;
			if (encoding == 4)
				encoding = MPG123_ENC_SIGNED_32;
			iRes = mpg123_format(m_handle, sFMT.wBitsPerSample, sFMT.nChannels, encoding);
			if (iRes != MPG123_OK)
				return FALSE;
		}
		return TRUE;
	}
	WAVEFORMATEX* MPG123Decode::GetFormat()
	{
		return reinterpret_cast<WAVEFORMATEX*>(&m_waveFMT);
	}
}
