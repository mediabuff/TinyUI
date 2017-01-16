#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11TextureShader.h"
#include "DX11Image.h"
#include <Richedit.h>

namespace DXFramework
{
	void CHARFORMAT2LOGFONT(const CHARFORMAT& cf, LOGFONT& lf, COLORREF& color);
	void LOGFONT2CHARFORMAT(const LOGFONT& lf, CHARFORMAT& cf, const COLORREF& color);
	Gdiplus::RectF WINAPI MeasureString(const wstring& str, const Gdiplus::Font* font);
	Gdiplus::RectF WINAPI MeasureString(const wstring& str, const CHARFORMAT& cf);
	/// <summary>
	/// DX11����
	/// </summary>
	class DX11Font : public DX11Image
	{
		DISALLOW_COPY_AND_ASSIGN(DX11Font)
	public:
		DX11Font();
		virtual ~DX11Font();
		BOOL Create(DX11& dx11, const wstring& str, const CHARFORMAT& cf, const COLORREF& bkColor);
		BOOL DrawString(DX11& dx11, const TinyString& str, const PointF& pos, const StringFormat* format);
	private:
		CHARFORMAT	m_cf;
		COLORREF	m_textColor;
		COLORREF	m_bkColor;
	};
}



