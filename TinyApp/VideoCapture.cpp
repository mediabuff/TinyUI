#include "stdafx.h"
#include "VideoCapture.h"


CVideoCapture::CVideoCapture()
{

}

CVideoCapture::~CVideoCapture()
{

}

BOOL CVideoCapture::Initialize()
{
	return FALSE;
	//return m_d3dCaptureSource.Initialize("War3.exe");
}

DWORD CVideoCapture::MainCaptureLoop(LPVOID lpUnused)
{
	CVideoCapture* capture = reinterpret_cast<CVideoCapture*>(lpUnused);
	while (capture->m_video.Lock(INFINITE))
	{
		capture->m_video.SetEvent();
	}
	return FALSE;
}
