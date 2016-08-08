#include "../stdafx.h"
#include "TinyVisualHWND.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualRichText.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree),
			m_hscroll(NULL),
			m_vscroll(NULL)
		{

		}

		HRESULT TinyVisualRichText::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}
		HRESULT TinyVisualRichText::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage)
		{
			ASSERT(m_texthost.m_ts);
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			POINT pos;
			GetCursorPos(&pos);
			::ScreenToClient(hWND, &pos);
			HDC hDC = GetDC(hWND);
			if (hDC != NULL)
			{
				TinyRectangle clip = GetWindowRect();
				m_texthost.m_ts->OnTxSetCursor(DVASPECT_CONTENT, -1, NULL, NULL, hDC, NULL, &clip, pos.x, pos.y);
				ReleaseDC(hWND, hDC);
			}
			return TRUE;
		}

		HRESULT	TinyVisualRichText::OnFocus(BOOL bFlag)
		{
			ASSERT(m_texthost.m_ts);
			if (bFlag)
			{
				TinyRectangle clip = GetWindowRect();
				m_texthost.m_ts->OnTxInPlaceActivate(clip);
				m_texthost.m_ts->OnTxUIActivate();
				m_texthost.m_ts->TxSendMessage(WM_SETFOCUS, 0, 0, NULL);
			}
			else
			{
				m_texthost.m_ts->OnTxInPlaceDeactivate();
				m_texthost.m_ts->OnTxUIDeactivate();
				m_texthost.m_ts->TxSendMessage(WM_KILLFOCUS, 0, 0, NULL);
			}
			return FALSE;
		}

		HRESULT TinyVisualRichText::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes);
		}

		TinyVisualRichText::~TinyVisualRichText()
		{

		}
		TinyString TinyVisualRichText::RetrieveTag() const
		{
			return TinyVisualTag::RICHTEXT;
		}

		BOOL TinyVisualRichText::OnFilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
		{
			if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
				return FALSE;
			if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
				return FALSE;
			if (uMsg == WM_CHAR || uMsg == WM_SYSCHAR)
				return FALSE;
			if (uMsg == WM_SETCURSOR)
				return FALSE;
			if (IsFocus())
			{
				return m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lResult) == S_OK;
			}
			return FALSE;
		}

		BOOL TinyVisualRichText::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_texthost.m_ts);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			::SetGraphicsMode(canvas, GM_COMPATIBLE);
			TinyRectangle clip = GetWindowRect();
			m_texthost.m_ts->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, canvas, NULL, reinterpret_cast<LPCRECTL>(&clip), NULL, reinterpret_cast<LPRECT>(&clip), NULL, 0, 0);
			return TRUE;
		}
		HRESULT TinyVisualRichText::OnCreate()
		{
			TinySize size = this->GetSize();
			m_hscroll = static_cast<TinyVisualHScrollBar*>(m_document->GetFactory()->Create(0, size.cy - 12, size.cx, 12, this, TinyVisualTag::HSCROLLBAR));
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualRichText::OnPosChange));
			m_hscroll->EVENT_PosChange += m_onPosChange;
			m_hscroll->SetVisible(FALSE);
			m_vscroll = static_cast<TinyVisualVScrollBar*>(m_document->GetFactory()->Create(size.cx - 12, 0, 12, size.cy, this, TinyVisualTag::VSCROLLBAR));
			m_onPosChange.Reset(new Delegate<void(BOOL, INT, INT, INT)>(this, &TinyVisualRichText::OnPosChange));
			m_vscroll->EVENT_PosChange += m_onPosChange;
			m_vscroll->SetVisible(FALSE);
			m_texthost.Initialize(this);
			m_texthost.UpdateView();
			m_document->GetVisualHWND()->AddFilter(this);
			return S_OK;
		}
		HRESULT TinyVisualRichText::OnDestory()
		{
			m_hscroll->EVENT_PosChange -= m_onPosChange;
			m_vscroll->EVENT_PosChange -= m_onPosChange;
			m_document->GetVisualHWND()->RemoveFilter(this);
			return S_OK;
		}

		void TinyVisualRichText::OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos)
		{
			ASSERT(m_texthost.m_ts);
			if (bVer)
			{
				LRESULT lRes = 0;
				m_texthost.m_ts->TxSendMessage(WM_VSCROLL, MAKEWPARAM(code, iNewPos), 0, &lRes);
			}
			else
			{
				LRESULT lRes = 0;
				m_texthost.m_ts->TxSendMessage(WM_HSCROLL, MAKEWPARAM(code, iNewPos), 0, &lRes);
			}
		}



		BOOL TinyVisualRichText::SetReadonly(BOOL bFlag)
		{
			ASSERT(m_texthost.m_ts);
			return FALSE;
		}
	}
}
