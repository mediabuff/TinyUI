#include "stdafx.h"
#include "x264Decode.h"

namespace Decode
{
	x264Decode::x264Decode()
		:m_pYUV420(NULL),
		m_context(NULL),
		m_codec(NULL),
		m_sws(NULL),
		m_pRGB32(NULL)
	{
		avcodec_register_all();
		ZeroMemory(&m_packet, sizeof(m_packet));
	}

	x264Decode::~x264Decode()
	{
		Close();
	}

	BOOL x264Decode::Initialize(const TinySize& srcsize, const TinySize& dstsize)
	{
		avcodec_register_all();
		av_init_packet(&m_packet);
		m_srcsize = srcsize;
		m_dstsize = dstsize;
		m_pYUV420 = av_frame_alloc();
		if (!m_pYUV420)
			goto _ERROR;
		m_pRGB32 = av_frame_alloc();
		if (!m_pRGB32)
			goto _ERROR;
		m_sws = sws_getContext(srcsize.cx, srcsize.cy, AV_PIX_FMT_YUV420P, dstsize.cx, dstsize.cy, AV_PIX_FMT_RGB32, 0, NULL, NULL, NULL);
		if (!m_sws)
			goto _ERROR;
		INT size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, m_dstsize.cx, m_dstsize.cy, 1);
		m_bits.Reset(new BYTE[size]);
		if (!m_bits)
			goto _ERROR;
		memset(m_bits, 0, size);
		return size == av_image_fill_arrays(m_pRGB32->data, m_pRGB32->linesize, m_bits.Ptr(), AV_PIX_FMT_RGB32, m_dstsize.cx, m_dstsize.cy, 1);
	_ERROR:
		Close();
		return FALSE;

	}
	BOOL x264Decode::Open(BYTE* metadata, LONG size)
	{
		m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
		if (!m_codec)
			goto H264_ERROR;
		m_context = avcodec_alloc_context3(m_codec);
		if (!m_context)
			goto H264_ERROR;
		m_context->flags2 |= CODEC_FLAG2_CHUNKS | CODEC_FLAG2_FAST;
		if (m_codec->capabilities & CODEC_CAP_TRUNCATED)
		{
			m_context->flags |= CODEC_FLAG_TRUNCATED;
		}
		m_context->extradata_size = size;
		m_context->extradata = reinterpret_cast<BYTE*>(av_malloc(m_context->extradata_size));
		memcpy(m_context->extradata, metadata, m_context->extradata_size);
		if (avcodec_open2(m_context, m_codec, NULL) != 0)
			goto H264_ERROR;
		return TRUE;
	H264_ERROR:
		Close();
		return FALSE;
	}
	BOOL x264Decode::Decode(SampleTag& tag, BYTE*& bo, LONG& so)
	{
		if (!m_context || !m_sws)
			return FALSE;
		so = 0;
		bo = NULL;
		m_packet.dts = tag.sampleDTS;
		m_packet.pts = tag.samplePTS;
		m_packet.data = tag.bits;
		m_packet.size = tag.size;
		INT iRes = avcodec_send_packet(m_context, &m_packet);
		if (iRes != 0)
			goto _ERROR;
		while (iRes >= 0)
		{
			iRes = avcodec_receive_frame(m_context, m_pYUV420);
			if (iRes == AVERROR(EAGAIN) || iRes == AVERROR_EOF)
				break;
			else if (iRes < 0)
			{
				goto _ERROR;
			}
			INT cy = sws_scale(m_sws, m_pYUV420->data, m_pYUV420->linesize, 0, m_srcsize.cy, m_pRGB32->data, m_pRGB32->linesize);
			ASSERT(cy == m_dstsize.cy);
			bo = m_pRGB32->data[0];
			so = m_pRGB32->linesize[0] * cy;
			goto _ERROR;
		}
	_ERROR:
		SAFE_DELETE_ARRAY(tag.bits);
		av_packet_unref(&m_packet);
		return iRes == 0;
	}
	BOOL x264Decode::Close()
	{
		m_srcsize.Empty();
		m_dstsize.Empty();
		if (m_context != NULL)
		{
			if (m_context->extradata)
			{
				av_free(m_context->extradata);
				m_context->extradata = NULL;
			}
			avcodec_close(m_context);
			avcodec_free_context(&m_context);
			m_context = NULL;
			m_codec = NULL;
		}
		if (m_sws != NULL)
		{
			sws_freeContext(m_sws);
			m_sws = NULL;
		}
		if (m_pYUV420 != NULL)
		{
			av_frame_free(&m_pYUV420);
			m_pYUV420 = NULL;
		}
		if (m_pRGB32 != NULL)
		{
			av_frame_free(&m_pRGB32);
			m_pRGB32 = NULL;
		}
		m_bits.Reset(NULL);
		return TRUE;
	}
	AVFrame* x264Decode::GetYUV420() const
	{
		return m_pYUV420;
	}
	AVFrame* x264Decode::GetRGB32() const
	{
		return m_pRGB32;
	}
}
