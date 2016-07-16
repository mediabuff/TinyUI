#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualWindow::TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{
			TinyVisualHWND* pWND = vtree->GetVisualHWND();
			if (pWND)
			{
				pWND->GetClientRect(&m_rectangle);
			}
		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}
		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTag::WINDOW;
		}
		BOOL TinyVisualWindow::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document || m_document->GetVisualHWND());
			TinyImage& image = m_images[NORMAL];
			if (image.IsEmpty())
				return FALSE;
			TinyClipCanvas canvas(hDC, this, rcPaint);
			canvas.DrawImage(image, m_rectangle, 0, 0, image.GetSize().cx, image.GetSize().cy);
			return TRUE;
		}
	}
}
