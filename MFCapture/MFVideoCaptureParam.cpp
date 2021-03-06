#include "stdafx.h"
#include "MFVideoCaptureParam.h"

namespace MF
{
	MFVideoCaptureParam::MFVideoCaptureParam()
		:m_rate(0.0F), m_vpf(PIXEL_FORMAT_UNKNOWN), m_dwIndex(0)
	{

	}
	MFVideoCaptureParam::MFVideoCaptureParam(const TinySize& size, FLOAT rate, VideoPixelFormat vpf, DWORD dwIndex)
		: m_size(size),
		m_rate(rate),
		m_vpf(vpf),
		m_dwIndex(dwIndex)
	{

	}
	string MFVideoCaptureParam::PixelFormatToString(VideoPixelFormat vpf)
	{
		switch (vpf)
		{
		case PIXEL_FORMAT_UNKNOWN:
			return "UNKNOW";
		case PIXEL_FORMAT_I420:
			return "I420";
		case PIXEL_FORMAT_YV12:
			return "YV12";
		case PIXEL_FORMAT_YV16:
			return "YV16";
		case PIXEL_FORMAT_YV12A:
			return "YV12A";
		case PIXEL_FORMAT_YV24:
			return "YV24";
		case PIXEL_FORMAT_NV12:
			return "NV12";
		case PIXEL_FORMAT_NV21:
			return "NV21";
		case PIXEL_FORMAT_UYVY:
			return "UYVY";
		case PIXEL_FORMAT_YUY2:
			return "YUY2";
		case PIXEL_FORMAT_ARGB:
			return "ARGB";
		case PIXEL_FORMAT_XRGB:
			return "XRGB";
		case PIXEL_FORMAT_RGB24:
			return "RGB24";
		case PIXEL_FORMAT_RGB32:
			return "RGB32";
		case PIXEL_FORMAT_MJPEG:
			return "MJPEG";
		case PIXEL_FORMAT_MT21:
			return "MT21";
		case PIXEL_FORMAT_YUV420P9:
			return "YUV420P9";
		case PIXEL_FORMAT_YUV420P10:
			return "YUV420P10";
		case PIXEL_FORMAT_YUV422P9:
			return "YUV422P9";
		case PIXEL_FORMAT_YUV422P10:
			return "YUV422P10";
		case PIXEL_FORMAT_YUV444P9:
			return "YUV444P9";
		case PIXEL_FORMAT_YUV444P10:
			return "YUV444P10";
		}
		return "";
	}
	BOOL MFVideoCaptureParam::IsValid() const
	{
		return (m_size.cx < ((1 << 15) - 1)) &&
			(m_size.cy < ((1 << 15) - 1)) &&
			(m_size.cx * m_size.cy >= 0) &&
			(m_size.cx * m_size.cy < (1 << (14 * 2))) &&
			(m_rate >= 0.0F) &&
			(m_rate < 1000) &&
			(m_vpf >= PIXEL_FORMAT_UNKNOWN) &&
			(m_vpf < PIXEL_FORMAT_MAX);
	}
	void MFVideoCaptureParam::SetFormat(VideoPixelFormat vpf)
	{
		m_vpf = vpf;
	}
	void MFVideoCaptureParam::SetSize(const TinySize& size)
	{
		m_size = size;
	}
	void MFVideoCaptureParam::SetSize(INT cx, INT cy)
	{
		m_size.cx = cx;
		m_size.cy = cy;
	}
	void MFVideoCaptureParam::SetScale(INT cx, INT cy)
	{
		m_scale.cx = cx;
		m_scale.cy = cy;
	}
	void MFVideoCaptureParam::SetRate(FLOAT rate)
	{
		m_rate = rate;
	}
	void MFVideoCaptureParam::SetStreamIndex(DWORD dwIndex)
	{
		m_dwIndex = dwIndex;
	}
	const TinySize& MFVideoCaptureParam::GetSize() const
	{
		return m_size;
	}
	const TinySize&	MFVideoCaptureParam::GetScale() const
	{
		return m_scale;
	}
	const DWORD MFVideoCaptureParam::GetStreamIndex() const
	{
		return m_dwIndex;
	}
	FLOAT MFVideoCaptureParam::GetRate() const
	{
		return m_rate;
	}
	VideoPixelFormat MFVideoCaptureParam::GetFormat() const
	{
		return m_vpf;
	}
	string MFVideoCaptureParam::ToString() const
	{
		return StringPrintf("%s,%d*%d,%.3f,%d", PixelFormatToString(m_vpf).c_str(), m_size.cx, m_size.cy, m_rate, m_dwIndex);
	}
}
