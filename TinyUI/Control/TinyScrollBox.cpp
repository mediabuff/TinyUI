#include "../stdafx.h"
#include "TinyScrollBox.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyScrollBox, TinyControl);
	TinyScrollBox::TinyScrollBox()
		:m_bTracking(FALSE),
		m_bMouseTracking(FALSE)
	{
		memset(&m_si, 0, sizeof(SCROLLBOXINFO));
		m_si.iTrackHitTest = m_si.iLatestHitTest = HTSCROLL_NONE;
	}

	TinyScrollBox::~TinyScrollBox()
	{

	}

	BOOL TinyScrollBox::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	LPCSTR TinyScrollBox::RetrieveClassName()
	{
		return TEXT("TinyScrollBox");
	}
	LRESULT TinyScrollBox::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		PAINTSTRUCT ps = { 0 };
		HDC hDC = BeginPaint(m_hWND, &ps);
		SetGraphicsMode(hDC, GM_ADVANCED);

		TinyMemDC memdc(hDC, m_size.cx, m_size.cy);
		RECT paintRC = { 0, 0, m_size.cx, m_size.cy };
		FillRect(memdc, &paintRC, (HBRUSH)GetStockObject(WHITE_BRUSH));

		DrawScrollBar(memdc, m_si.iTrackHitTest, FALSE);

		memdc.Render(paintRC, paintRC, FALSE);
		EndPaint(m_hWND, &ps);
		return FALSE;
	}

	LRESULT TinyScrollBox::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return TRUE;
	}
	LRESULT TinyScrollBox::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		switch (wParam)
		{
		case SB_TIMER_SCROLL:
		{
			if (m_si.iTrackHitTest == m_si.iLatestHitTest)
			{
				DWORD dwPos = GetMessagePos();
				POINT pt = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
				::ScreenToClient(m_hWND, &pt);
				INT iHitTest = ScrollHitTest(pt);
				if (iHitTest == HTSCROLL_THUMB)
				{
					KillTimer(m_hWND, SB_TIMER_SCROLL);
					m_si.iTrackHitTest = HTSCROLL_THUMB;
					SCROLLCALC si = { 0 };
					ScrollCalculate(&si);
					m_si.iThumbOffset = pt.y - si.thumbRectangle.top;
					return FALSE;
				}
				INT iNewPos = m_si.iPos;
				switch (m_si.iTrackHitTest)
				{
				case HTSCROLL_NONE:
					KillTimer(m_hWND, SB_TIMER_SCROLL);
					return FALSE;
				case HTSCROLL_LINEUP:
					iNewPos = (m_si.iPos - 1) > m_si.iMin ? m_si.iPos - 1 : m_si.iMin;
					break;
				case HTSCROLL_LINEDOWN:
					iNewPos = (m_si.iPos + 1) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + 1 : (m_si.iMax - m_si.iPage + 1);
					break;
				case HTSCROLL_PAGEUP:
					iNewPos = (m_si.iPos - m_si.iPage) > m_si.iMin ? m_si.iPos - m_si.iPage : m_si.iMin;
					break;
				case HTSCROLL_PAGEDOWN:
					iNewPos = (m_si.iPos + m_si.iPage) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + m_si.iPage : (m_si.iMax - m_si.iPage + 1);
					break;
				}
				if (iNewPos != m_si.iPos)
				{
					PosChange(m_si.iPos, iNewPos);
					m_si.iPos = iNewPos;
				}

				HDC hDC = GetDC(m_hWND);
				TinyMemDC memdc(hDC, m_size.cx, m_size.cy);
				RECT paintRC = { 0, 0, m_size.cx, m_size.cy };
				FillRect(memdc, &paintRC, (HBRUSH)GetStockObject(WHITE_BRUSH));
				DrawScrollBar(memdc, m_si.iTrackHitTest, TRUE);
				memdc.Render(paintRC, paintRC, FALSE);
				ReleaseDC(m_hWND, hDC);
			}
		}
		break;
		case SB_TIMER_DELAY:
		{
			KillTimer(m_hWND, SB_TIMER_DELAY);
			SetTimer(m_hWND, SB_TIMER_SCROLL, SB_SCROLL_INTERVAL, 0);
		}
		break;
		}
		return FALSE;
	}
	LRESULT TinyScrollBox::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		if (!m_bMouseTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWND;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 0;
			m_bMouseTracking = _TrackMouseEvent(&tme);
		}

		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if (m_bTracking && (wParam & MK_LBUTTON))
		{
			if (m_si.iTrackHitTest == HTSCROLL_THUMB)
			{
				m_si.iLatestHitTest = HTSCROLL_THUMB;
				SCROLLCALC si = { 0 };
				ScrollCalculate(&si);
				ScrollTrackThumb(pt, &si);
				HDC hDC = GetDC(m_hWND);
				TinyMemDC memdc(hDC, m_size.cx, m_size.cy);
				RECT paintRC = { 0, 0, m_size.cx, m_size.cy };
				FillRect(memdc, &paintRC, (HBRUSH)GetStockObject(WHITE_BRUSH));
				DrawScrollBar(memdc, HTSCROLL_THUMB, wParam & MK_LBUTTON);
				memdc.Render(paintRC, paintRC, FALSE);
				ReleaseDC(m_hWND, hDC);
				return FALSE;
			}
		}
		INT iHitTest = ScrollHitTest(pt);
		if (m_si.iLatestHitTest != iHitTest)
		{
			HDC hDC = GetDC(m_hWND);
			TinyMemDC memdc(hDC, m_size.cx, m_size.cy);
			RECT paintRC = { 0, 0, m_size.cx, m_size.cy };
			FillRect(memdc, &paintRC, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawScrollBar(memdc, iHitTest, wParam & MK_LBUTTON);
			memdc.Render(paintRC, paintRC, FALSE);
			ReleaseDC(m_hWND, hDC);
			m_si.iLatestHitTest = iHitTest;
		}

		return FALSE;
	}
	LRESULT TinyScrollBox::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		INT iHitTest = ScrollHitTest(pt);
		switch (iHitTest)
		{
		case HTSCROLL_LINEUP:
		case HTSCROLL_LINEDOWN:
		case HTSCROLL_PAGEUP:
		case HTSCROLL_PAGEDOWN:
			SetTimer(m_hWND, SB_TIMER_DELAY, SB_SCROLL_DELAY, NULL);
			break;
		case HTSCROLL_THUMB:
			SCROLLCALC si = { 0 };
			ScrollCalculate(&si);
			m_si.iThumbOffset = pt.y - si.thumbRectangle.top;
			break;
		}
		m_si.iLatestHitTest = iHitTest;
		m_si.iTrackHitTest = iHitTest;
		m_bTracking = TRUE;

		HDC hDC = GetDC(m_hWND);
		TinyMemDC memdc(hDC, m_size.cx, m_size.cy);
		RECT paintRC = { 0, 0, m_size.cx, m_size.cy };
		FillRect(memdc, &paintRC, (HBRUSH)GetStockObject(WHITE_BRUSH));
		DrawScrollBar(memdc, iHitTest, TRUE);
		memdc.Render(paintRC, paintRC, FALSE);
		ReleaseDC(m_hWND, hDC);
		SetCapture(m_hWND);

		return FALSE;
	}
	LRESULT TinyScrollBox::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_bTracking)
		{
			INT iNewPos = m_si.iPos;
			switch (m_si.iTrackHitTest)
			{
			case HTSCROLL_LINEUP:
				iNewPos = (m_si.iPos - 1) > m_si.iMin ? m_si.iPos - 1 : m_si.iMin;
				KillTimer(m_hWND, SB_TIMER_SCROLL);
				break;
			case HTSCROLL_LINEDOWN:
				iNewPos = (m_si.iPos + 1) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + 1 : (m_si.iMax - m_si.iPage + 1);
				KillTimer(m_hWND, SB_TIMER_SCROLL);
				break;
			case HTSCROLL_PAGEUP:
				iNewPos = (m_si.iPos - m_si.iPage) > m_si.iMin ? m_si.iPos - m_si.iPage : m_si.iMin;
				KillTimer(m_hWND, SB_TIMER_SCROLL);
				break;
			case HTSCROLL_PAGEDOWN:
				iNewPos = (m_si.iPos + m_si.iPage) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + m_si.iPage : (m_si.iMax - m_si.iPage + 1);
				KillTimer(m_hWND, SB_TIMER_SCROLL);
				break;
			}
			if (iNewPos != m_si.iPos)
			{
				PosChange(m_si.iPos, iNewPos);
				m_si.iPos = iNewPos;
			}

			HDC hDC = GetDC(m_hWND);
			TinyMemDC memdc(hDC, m_size.cx, m_size.cy);
			RECT paintRC = { 0, 0, m_size.cx, m_size.cy };
			FillRect(memdc, &paintRC, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawScrollBar(memdc, HTSCROLL_THUMB, FALSE);
			memdc.Render(paintRC, paintRC, FALSE);
			ReleaseDC(m_hWND, hDC);

			m_bTracking = FALSE;
		}
		m_si.iLatestHitTest = HTSCROLL_NONE;
		m_si.iTrackHitTest = HTSCROLL_NONE;
		m_si.iThumbOffset = 0;

		ReleaseCapture();

		return FALSE;
	}
	LRESULT TinyScrollBox::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		//上箭头
		m_images[0].Open("..\\Resource\\ScrollBar\\arrow_up_normal.png");
		m_images[1].Open("..\\Resource\\ScrollBar\\arrow_up_hover.png");
		m_images[2].Open("..\\Resource\\ScrollBar\\arrow_up_press.png");
		//下箭头
		m_images[3].Open("..\\Resource\\ScrollBar\\arrow_down_normal.png");
		m_images[4].Open("..\\Resource\\ScrollBar\\arrow_down_hover.png");
		m_images[5].Open("..\\Resource\\ScrollBar\\arrow_down_press.png");
		//划块
		m_images[6].Open("..\\Resource\\ScrollBar\\scrollbar_groove.png");
		m_images[7].Open("..\\Resource\\ScrollBar\\scrollbar_normal.png");
		m_images[8].Open("..\\Resource\\ScrollBar\\scrollbar_hover.png");

		return FALSE;
	}
	LRESULT TinyScrollBox::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_size.cx = LOWORD(lParam);
		m_size.cy = HIWORD(lParam);
		return FALSE;
	}
	LRESULT TinyScrollBox::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		if (m_bMouseTracking)
		{
			m_bMouseTracking = FALSE;

			if (m_bTracking)
			{
				KillTimer(m_hWND, SB_TIMER_SCROLL);
				KillTimer(m_hWND, SB_TIMER_DELAY);
				return FALSE;
			}

			m_si.iTrackHitTest = m_si.iLatestHitTest = HTSCROLL_NONE;
			KillTimer(m_hWND, SB_TIMER_SCROLL);
			KillTimer(m_hWND, SB_TIMER_DELAY);

			HDC hDC = GetDC(m_hWND);
			TinyMemDC memdc(hDC, m_size.cx, m_size.cy);
			RECT paintRC = { 0, 0, m_size.cx, m_size.cy };
			FillRect(memdc, &paintRC, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawScrollBar(memdc, HTSCROLL_NONE, FALSE);
			memdc.Render(paintRC, paintRC, FALSE);
			ReleaseDC(m_hWND, hDC);
		}

		return FALSE;
	}
	LRESULT TinyScrollBox::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		SHORT zDelta = (SHORT)HIWORD(wParam) / 10;
		INT iNewPos = m_si.iPos;
		if (zDelta > 0)
		{
			iNewPos = (m_si.iPos - abs(zDelta)) > m_si.iMin ? m_si.iPos - abs(zDelta) : m_si.iMin;
		}
		else
		{
			iNewPos = (m_si.iPos + abs(zDelta)) < (m_si.iMax - m_si.iPage + 1) ? m_si.iPos + abs(zDelta) : (m_si.iMax - m_si.iPage + 1);
		}
		if (iNewPos != m_si.iPos)
		{
			PosChange(m_si.iPos, iNewPos);
			m_si.iPos = iNewPos;
		}
		DWORD dwPos = GetMessagePos();
		POINT pt = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
		::ScreenToClient(m_hWND, &pt);
		m_si.iTrackHitTest = m_si.iLatestHitTest = ScrollHitTest(pt);
		HDC hDC = GetDC(m_hWND);
		TinyMemDC memdc(hDC, m_size.cx, m_size.cy);
		RECT paintRC = { 0, 0, m_size.cx, m_size.cy };
		FillRect(memdc, &paintRC, (HBRUSH)GetStockObject(WHITE_BRUSH));
		DrawScrollBar(memdc, HTSCROLL_THUMB, FALSE);
		memdc.Render(paintRC, paintRC, FALSE);
		ReleaseDC(m_hWND, hDC);
		return FALSE;
	}
	INT	TinyScrollBox::ScrollHitTest(POINT& pt)
	{
		SCROLLCALC si = { 0 };
		ScrollCalculate(&si);
		if (pt.y >= si.arrowRectangle[0].top && pt.y < si.arrowRectangle[0].bottom)
			return HTSCROLL_LINEUP;
		if (pt.y >= si.pageRectangle[0].top && pt.y < si.pageRectangle[0].bottom)
			return HTSCROLL_PAGEUP;
		if (pt.y >= si.thumbRectangle.top && pt.y < si.thumbRectangle.bottom)
			return HTSCROLL_THUMB;
		if (pt.y >= si.pageRectangle[1].top && pt.y < si.pageRectangle[1].bottom)
			return HTSCROLL_PAGEDOWN;
		if (pt.y >= si.arrowRectangle[1].top && pt.y < si.arrowRectangle[1].bottom)
			return HTSCROLL_LINEDOWN;
		return HTSCROLL_NONE;
	}
	void TinyScrollBox::ScrollCalculate(SCROLLCALC* ps)
	{
		ASSERT(ps);
		SetRect(&ps->rectangle,
			TINY_SCROLL_MARGIN_LEFT,
			TINY_SCROLL_MARGIN_TOP,
			m_size.cx - TINY_SCROLL_MARGIN_RIGHT,
			m_size.cy - TINY_SCROLL_MARGIN_BOTTOM);
		SetRect(&ps->arrowRectangle[0],
			ps->rectangle.left,
			ps->rectangle.top,
			ps->rectangle.right,
			ps->rectangle.top + TINY_SM_CYSCROLL);//上箭头的高度
		SetRect(&ps->arrowRectangle[1],
			ps->rectangle.left,
			ps->rectangle.bottom - TINY_SM_CYSCROLL,
			ps->rectangle.right,
			ps->rectangle.bottom);//下箭头的大小
		INT iRange = (m_si.iMax - m_si.iMin) + 1;
		INT iGrooveSize = ps->arrowRectangle[1].top - ps->arrowRectangle[0].bottom;//划块的槽
		BOOL bFlag = (m_si.iPage > m_si.iMax || m_si.iMax <= m_si.iMin || m_si.iMax == 0);
		if (iRange > 0 && !bFlag)
		{
			//计算划块的大小
			INT iThumbPos = 0;
			INT iThumbSize = MulDiv(m_si.iPage, iGrooveSize, iRange);
			if (iThumbSize < TINY_SCROLL_MINTHUMB_SIZE)
				iThumbSize = TINY_SCROLL_MINTHUMB_SIZE;
			//计算Page
			INT iPageSize = max(1, m_si.iPage);
			iThumbPos = MulDiv(m_si.iPos - m_si.iMin, iGrooveSize - iThumbSize, iRange - iPageSize);
			if (iThumbPos < 0)
				iThumbPos = 0;
			if (iThumbPos >= (iGrooveSize - iThumbSize))
				iThumbPos = iGrooveSize - iThumbSize;
			iThumbPos += ps->rectangle.top + TO_CY(ps->arrowRectangle[0]);
			SetRect(&ps->thumbRectangle,
				ps->rectangle.left,
				iThumbPos,
				ps->rectangle.right,
				iThumbPos + iThumbSize);
			SetRect(&ps->pageRectangle[0],
				ps->rectangle.top,
				ps->arrowRectangle[0].bottom,
				ps->rectangle.right,
				ps->thumbRectangle.top);
			SetRect(&ps->pageRectangle[1],
				ps->rectangle.top,
				ps->thumbRectangle.top,
				ps->rectangle.right,
				ps->arrowRectangle[1].top);
		}
		else
		{
			SetRect(&ps->thumbRectangle,
				ps->rectangle.left,
				0,
				ps->rectangle.right,
				0);
			SetRect(&ps->pageRectangle[0],
				ps->rectangle.top,
				ps->arrowRectangle[0].bottom,
				ps->rectangle.right,
				ps->arrowRectangle[1].top);
			SetRect(&ps->pageRectangle[1],
				ps->rectangle.top,
				ps->arrowRectangle[0].bottom,
				ps->rectangle.right,
				ps->arrowRectangle[1].top);
		}
	}
	void TinyScrollBox::ScrollTrackThumb(POINT& pt, SCROLLCALC* ps)
	{
		ASSERT(ps);
		INT iPos = 0;
		INT iRange = (m_si.iMax - m_si.iMin) + 1;
		INT iThumbPos = (pt.y - m_si.iThumbOffset) - ps->arrowRectangle[0].bottom;
		INT iThumbSize = TO_CY(ps->thumbRectangle);
		INT iGrooveSize = ps->arrowRectangle[1].top - ps->arrowRectangle[0].bottom;
		if (iThumbPos < 0)
			iThumbPos = 0;
		if (iThumbPos >(iGrooveSize - iThumbSize))
			iThumbPos = iGrooveSize - iThumbSize;

		if (iRange > 0)
		{
			iPos = m_si.iMin + MulDiv(iThumbPos, iRange - m_si.iPage, iGrooveSize - iThumbSize);
		}
		if (m_si.iPos != iPos)
		{
			INT iOldPos = m_si.iPos;
			m_si.iPos = iPos;
			PosChange(iOldPos, iPos);
		}
	}
	void TinyScrollBox::DrawScrollBar(TinyMemDC& dc, INT iHitTest, BOOL bMouseDown)
	{
		SCROLLCALC si = { 0 };
		ScrollCalculate(&si);
		DrawArrow(dc, &si, iHitTest, bMouseDown);
		DrawGroove(dc, &si);
		DrawThumb(dc, &si, iHitTest);
	}
	void TinyScrollBox::DrawArrow(TinyMemDC& dc, SCROLLCALC* ps, INT iHitTest, BOOL bMouseDown)
	{
		if (iHitTest == HTSCROLL_LINEUP)
		{
			if (bMouseDown)
			{
				TinyMemDC memdc1(dc, m_images[2]);
				memdc1.Render(ps->arrowRectangle[0], m_images[2].GetRectangle(), TRUE);
				TinyMemDC memdc2(dc, m_images[3]);
				memdc2.Render(ps->arrowRectangle[1], m_images[3].GetRectangle(), TRUE);
			}
			else
			{
				TinyMemDC memdc1(dc, m_images[1]);
				memdc1.Render(ps->arrowRectangle[0], m_images[1].GetRectangle(), TRUE);
				TinyMemDC memdc2(dc, m_images[3]);
				memdc2.Render(ps->arrowRectangle[1], m_images[3].GetRectangle(), TRUE);
			}
		}
		else if (iHitTest == HTSCROLL_LINEDOWN)
		{
			if (bMouseDown)
			{
				TinyMemDC memdc1(dc, m_images[0]);
				memdc1.Render(ps->arrowRectangle[0], m_images[0].GetRectangle(), TRUE);
				TinyMemDC memdc2(dc, m_images[5]);
				memdc2.Render(ps->arrowRectangle[1], m_images[5].GetRectangle(), TRUE);
			}
			else
			{
				TinyMemDC memdc1(dc, m_images[0]);
				memdc1.Render(ps->arrowRectangle[0], m_images[0].GetRectangle(), TRUE);
				TinyMemDC memdc2(dc, m_images[4]);
				memdc2.Render(ps->arrowRectangle[1], m_images[4].GetRectangle(), TRUE);
			}
		}
		else
		{
			TinyMemDC memdc1(dc, m_images[0]);
			memdc1.Render(ps->arrowRectangle[0], m_images[0].GetRectangle(), TRUE);
			TinyMemDC memdc2(dc, m_images[3]);
			memdc2.Render(ps->arrowRectangle[1], m_images[3].GetRectangle(), TRUE);
		}
	}
	void TinyScrollBox::DrawThumb(TinyMemDC& dc, SCROLLCALC* ps, INT iHitTest)
	{
		if (iHitTest == HTSCROLL_THUMB)
		{
			TinyMemDC memdc(dc, m_images[8]);
			RECT dstCenter, srcCenter;
			CopyRect(&srcCenter, &m_images[8].GetRectangle());
			srcCenter.top = srcCenter.top + 4;
			srcCenter.bottom = srcCenter.bottom - 4;
			CopyRect(&dstCenter, &ps->thumbRectangle);
			dstCenter.top = dstCenter.top + 4;
			dstCenter.bottom = dstCenter.bottom - 4;
			memdc.Render(ps->thumbRectangle, dstCenter, m_images[8].GetRectangle(), srcCenter, TRUE);
		}
		else
		{
			TinyMemDC memdc(dc, m_images[7]);
			RECT dstCenter, srcCenter;
			CopyRect(&srcCenter, &m_images[7].GetRectangle());
			srcCenter.top = srcCenter.top + 4;
			srcCenter.bottom = srcCenter.bottom - 4;
			CopyRect(&dstCenter, &ps->thumbRectangle);
			dstCenter.top = dstCenter.top + 4;
			dstCenter.bottom = dstCenter.bottom - 4;
			memdc.Render(ps->thumbRectangle, dstCenter, m_images[7].GetRectangle(), srcCenter, TRUE);
		}
	}
	void TinyScrollBox::DrawGroove(TinyMemDC& dc, SCROLLCALC* ps)
	{
		TinyMemDC memdc(dc, m_images[7]);
		RECT dstCenter, srcCenter;
		CopyRect(&srcCenter, &m_images[7].GetRectangle());
		srcCenter.top = srcCenter.top + 4;
		srcCenter.bottom = srcCenter.bottom - 4;
		RECT grooveRectangle;
		CopyRect(&grooveRectangle, &ps->rectangle);
		grooveRectangle.top = ps->arrowRectangle[0].bottom;
		grooveRectangle.bottom = ps->arrowRectangle[1].top;
		CopyRect(&dstCenter, &grooveRectangle);
		dstCenter.top = dstCenter.top + 4;
		dstCenter.bottom = dstCenter.bottom - 4;
		memdc.Render(grooveRectangle, dstCenter, m_images[7].GetRectangle(), srcCenter, TRUE);
	}
	void TinyScrollBox::SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos)
	{
		m_si.iPos = iPos < iMin ? iMin : iPos;
		m_si.iPos = iPos > (iMax - iPage + 1) ? (iMax - iPage + 1) : iPos;
		if (iMin <= m_si.iPos && m_si.iPos <= (iMax - iPage + 1))
		{
			m_si.iMin = iMin;
			m_si.iMax = iMax;
			m_si.iPage = iPage;
		}
		else
		{
			m_si.iPos = m_si.iMin = m_si.iMax = m_si.iMin = 0;
		}
		InvalidateRect(m_hWND, NULL, FALSE);
	}
	INT	TinyScrollBox::GetScrollPos()
	{
		return m_si.iPos;
	}
}
