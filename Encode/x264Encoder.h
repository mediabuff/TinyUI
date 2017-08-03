#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "Common/TinyEvent.h"
#include "Render/TinyGDI.h"
#include "Media/TinyMedia.h"
extern "C"
{
#include "x264_config.h"
#include "x264.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}
#pragma comment(lib,"libx264.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

#define SPS_SEP 4
#define PPS_SEP 4
using namespace TinyUI;
using namespace TinyUI::Media;

namespace Encode
{
	class x264Encoder
	{
		DISALLOW_COPY_AND_ASSIGN(x264Encoder)
	public:
		x264Encoder(Callback<void(BYTE*, LONG, const MediaTag&)>&& callback);
		virtual ~x264Encoder();
	public:
		BOOL	Open(INT cx, INT cy, INT videoFPS = 25, INT videoRate = 1000);//rateƽ������
		BOOL	Encode(AVFrame* pI420);
		void	Close();
	private:
		BOOL	BuildParam(INT cx, INT cy, INT fps, INT bitrate);
	private:
		x264_t*				m_x264;
		x264_param_t*		m_x264Param;
		x264_picture_t*		m_x264Image;
		LONGLONG			m_sINC;
		LONGLONG			m_sPTS;
		Callback<void(BYTE*, LONG, const MediaTag&)> m_callback;
	};
}
