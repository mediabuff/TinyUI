#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualCaption.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualCaption, TinyVisual);
		TinyVisualCaption::TinyVisualCaption(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualCaption::~TinyVisualCaption()
		{

		}
		TinyString TinyVisualCaption::RetrieveTag() const
		{
			return TinyVisualTag::SYSCAPTION;
		}
		void TinyVisualCaption::OnSizeChange(const TinySize& oldsize, const TinySize& newsize)
		{
			TinyVisual::OnSizeChange(oldsize, newsize);
		}

		BOOL TinyVisualCaption::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document || m_document->GetVisualHWND());
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
			{
				canvas.DrawImage(*m_backgroundImage, clip, 0, 0, m_backgroundImage->GetSize().cx, m_backgroundImage->GetSize().cy);
			}
			return TRUE;
		}

		HRESULT	TinyVisualCaption::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			HWND hWND = m_document->GetVisualHWND()->Handle();
			::SendMessage(hWND, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
			::SendMessage(hWND, WM_LBUTTONUP, dwFlags, MAKELPARAM(pos.x, pos.y));
			return TinyVisual::OnLButtonDown(pos, dwFlags);
		}
		HRESULT	TinyVisualCaption::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			return TinyVisual::OnMouseMove(pos, dwFlags);
		}
		HRESULT TinyVisualCaption::OnMouseLeave()
		{

			return TinyVisual::OnMouseLeave();
		}
		HRESULT	TinyVisualCaption::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			return TinyVisual::OnLButtonUp(pos, dwFlags);
		}
	}
}
