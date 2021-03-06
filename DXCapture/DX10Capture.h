#pragma once
#include "DX.h"

namespace DXCapture
{
	class DX10Capture
	{
	public:
		DX10Capture(DX& dx);
		~DX10Capture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Release();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Render(IDXGISwapChain *swap, UINT flags);
		BOOL DX10GPUHook(ID3D10Device *device);
	public:
		SharedCaptureDATA				m_captureDATA;
		DXGI_FORMAT						m_dxgiFormat;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		HANDLE							m_hTextureHandle;
		HMODULE							m_hD3D10;
		TinyComPtr<ID3D10Resource>		m_resource;
		TinyComPtr<ID3D10Texture2D>		m_copy2D;
		TinyDetour						m_dxPresent;
		TinyDetour						m_dxResizeBuffers;
		DX&								m_dx;
	};
	SELECTANY extern DX10Capture g_dx10(g_dx);
}

