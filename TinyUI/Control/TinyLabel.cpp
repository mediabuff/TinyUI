#include "../stdafx.h"
#include "TinyLabel.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyLabel, TinyControl);
	TinyLabel::TinyLabel()
	{

	}
	TinyLabel::~TinyLabel()
	{

	}
	LPCSTR TinyLabel::RetrieveClassName()
	{
		return WC_STATIC;
	}
	LPCSTR TinyLabel::RetrieveTitle()
	{
		return _T("TinyLabel");
	}
	DWORD TinyLabel::RetrieveStyle()
	{
		return (WS_EX_LEFT | WS_EX_LTRREADING | WS_CHILD | WS_VISIBLE | SS_WHITERECT | SS_NOTIFY);
	}
	DWORD TinyLabel::RetrieveExStyle()
	{
		return (WS_EX_LTRREADING);
	}
	BOOL TinyLabel::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		INITCOMMONCONTROLSEX initCtrls = { sizeof(INITCOMMONCONTROLSEX), ICC_STANDARD_CLASSES };
		if (InitCommonControlsEx(&initCtrls))
		{
			return TinyControl::Create(hParent, x, y, cx, cy);
		}
		return FALSE;
	}
	BOOL TinyLabel::SetText(LPCSTR pzText)
	{
		ASSERT(::IsWindow(m_hWND));
		return ::SetWindowText(m_hWND, pzText);
	}
	BOOL TinyLabel::GetText(LPSTR pzText, INT iSize)
	{
		ASSERT(::IsWindow(m_hWND));
		return ::GetWindowText(m_hWND, pzText, iSize);
	}
	HICON TinyLabel::SetIcon(HICON hIcon)
	{
		ASSERT(::IsWindow(m_hWND));
		return (HICON)::SendMessage(m_hWND, STM_SETICON, (WPARAM)hIcon, 0L);
	}
	HICON TinyLabel::GetIcon() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (HICON)::SendMessage(m_hWND, STM_GETICON, 0, 0L);
	}
	HENHMETAFILE TinyLabel::SetEnhMetaFile(HENHMETAFILE hMetaFile)
	{
		ASSERT(::IsWindow(m_hWND));
		return (HENHMETAFILE)::SendMessage(m_hWND, STM_SETIMAGE, IMAGE_ENHMETAFILE, (LPARAM)hMetaFile);
	}
	HENHMETAFILE TinyLabel::GetEnhMetaFile() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (HENHMETAFILE)::SendMessage(m_hWND, STM_GETIMAGE, IMAGE_ENHMETAFILE, 0L);
	}
	HBITMAP TinyLabel::SetBitmap(HBITMAP hBitmap)
	{
		ASSERT(::IsWindow(m_hWND));
		return (HBITMAP)::SendMessage(m_hWND, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
	}
	HBITMAP TinyLabel::GetBitmap() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (HBITMAP)::SendMessage(m_hWND, STM_GETIMAGE, IMAGE_BITMAP, 0L);
	}
	HCURSOR TinyLabel::SetCursor(HCURSOR hCursor)
	{
		ASSERT(::IsWindow(m_hWND));
		return (HCURSOR)::SendMessage(m_hWND, STM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor);
	}
	HCURSOR TinyLabel::GetCursor()
	{
		ASSERT(::IsWindow(m_hWND));
		return (HCURSOR)::SendMessage(m_hWND, STM_GETIMAGE, IMAGE_CURSOR, 0L);
	}

}