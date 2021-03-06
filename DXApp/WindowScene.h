#pragma once
#include "DX11.h"
#include "DX11Image2D.h"
#include "Common/TinyString.h"
#include <Richedit.h>
using namespace DXFramework;

namespace DXApp
{
	class WindowScene : public DX11Image2D
	{
		DECLARE_DYNAMIC(WindowScene)
	public:
		WindowScene();
		virtual ~WindowScene();
		BOOL Initialize(DX11& dx11, HWND hWND);
		HWND GetCaptureWindow() const;
	public:
		BOOL Allocate(DX11& dx11) OVERRIDE;
		BOOL Process(DX11& dx11) OVERRIDE;
		void Deallocate(DX11& dx11) OVERRIDE;
	private:
		HWND			m_hWND;
		BYTE*			m_bits;
		HBITMAP			m_hBitmap;
		TinyRectangle	m_snapshot;
		TinyScopedPtr<TinyMemDC> m_memDC;
	};
}



