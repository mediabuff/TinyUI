#include "stdafx.h"
#include "MPreviewController.h"
#include "WICTexture.h"
#include "MImageElement.h"
#include "MVideoElement.h"
#include "MShowApp.h"

namespace MShow
{
#define IDM_MOVEUP		100
#define IDM_MOVEDOWN	101
#define IDM_MOVETOP		102
#define IDM_MOVEBOTTPM	103
#define IDM_REMOVE		104

	MPreviewController::MPreviewController(MPreviewView& view)
		:m_view(view),
		m_current(NULL),
		m_bBreak(FALSE),
		m_bTracking(FALSE),
		m_bPopup(FALSE),
		m_renderView(m_graphics.GetDX11()),
		m_index(-1)
	{
		m_onLButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnLButtonDown));
		m_onLButtonUp.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnLButtonUp));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnRButtonDown));
		m_onMouseMove.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnMouseMove));
		m_onMouseLeave.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnMouseLeave));
		m_onSetCursor.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MPreviewController::OnSetCursor));
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &MPreviewController::OnMenuClick));
		m_view.EVENT_LBUTTONDOWN += m_onLButtonDown;
		m_view.EVENT_LBUTTONUP += m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE += m_onMouseMove;
		m_view.EVENT_MOUSELEAVE += m_onMouseLeave;
		m_view.EVENT_SETCURSOR += m_onSetCursor;
		m_popup.EVENT_CLICK += m_onMenuClick;
		m_signal.CreateEvent();
	}

	MPreviewController::~MPreviewController()
	{
		m_view.EVENT_LBUTTONDOWN -= m_onLButtonDown;
		m_view.EVENT_LBUTTONUP -= m_onLButtonUp;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_view.EVENT_MOUSEMOVE -= m_onMouseMove;
		m_view.EVENT_MOUSELEAVE -= m_onMouseLeave;
		m_view.EVENT_SETCURSOR -= m_onSetCursor;
		m_popup.DestroyMenu();
	}

	BOOL MPreviewController::Initialize()
	{
		TinyRectangle s;
		m_view.GetClientRect(&s);
		if (!m_graphics.Initialize(m_view.Handle(), s.Size()))
			return FALSE;
		string box;
		box.resize(MAX_PATH);
		GetModuleFileName(NULL, &box[0], MAX_PATH);
		box = box.substr(0, box.find_last_of("\\", string::npos, 1));
		string vs = box + "\\box.png";
		ASSERT(PathFileExists(vs.c_str()));
		for (INT i = 0;i < 8;i++)
		{
			m_handles[i].Load(m_graphics.GetDX11(), vs.c_str());
		}
		m_popup.CreatePopupMenu();
		m_popup.AppendMenu(MF_STRING, IDM_MOVEUP, TEXT("����"));
		m_popup.AppendMenu(MF_STRING, IDM_MOVEDOWN, TEXT("����"));
		m_popup.AppendMenu(MF_STRING, IDM_MOVETOP, TEXT("��������"));
		m_popup.AppendMenu(MF_STRING, IDM_MOVEBOTTPM, TEXT("�����ײ�"));
		m_popup.AppendMenu(MF_STRING, IDM_REMOVE, TEXT("�Ƴ�"));
		return TRUE;
	}

	BOOL MPreviewController::Add(DX11Element2D* ps)
	{
		TinyAutoLock lock(m_lock);
		if (m_array.Lookup(ps) < 0)
		{
			return m_array.Add(ps);
		}
		return FALSE;
	}

	BOOL MPreviewController::Remove(DX11Element2D* ps)
	{
		TinyAutoLock lock(m_lock);
		BOOL bRes = m_array.Remove(ps);
		if (ps == m_current)
		{
			m_current = NULL;
		}
		return bRes;
	}

	BOOL MPreviewController::Move(DX11Element2D* ps, BOOL bUp)
	{
		TinyAutoLock lock(m_lock);
		BOOL bRes = TRUE;
		if (bUp)
		{
			INT index = m_array.Lookup(ps);
			if (index >= 0 && index < (m_array.GetSize() - 1))
			{
				bRes &= m_array.Remove(ps);
				bRes &= m_array.Insert(index + 1, ps);
			}

		}
		else
		{
			INT index = m_array.Lookup(ps);
			if (index > 0)
			{
				bRes &= m_array.Remove(ps);
				bRes &= m_array.Insert(index - 1, ps);
			}
		}
		return bRes;
	}

	BOOL MPreviewController::Bring(DX11Element2D* ps, BOOL bTop)
	{
		TinyAutoLock lock(m_lock);
		BOOL bRes = TRUE;
		INT index = m_array.Lookup(ps);
		if (index >= 0)
		{
			m_array.Remove(ps);
			if (bTop)
			{
				bRes &= m_array.Add(ps);
			}
			else
			{
				bRes &= m_array.Insert(0, ps);
			}
		}
		return bRes;
	}
	BOOL MPreviewController::Find(DX11Element2D* ps)
	{
		TinyAutoLock lock(m_lock);
		return m_array.Lookup(ps) >= 0;
	}
	void MPreviewController::Lock()
	{
		m_lock.Lock();
	}
	void MPreviewController::Unlock()
	{
		m_lock.Unlock();
	}
	DX11Element2D* MPreviewController::HitTest(const TinyPoint& pos)
	{
		for (INT i = m_array.GetSize() - 1;i >= 0;i--)
		{
			if (m_array[i]->PtInRect(pos))
			{
				return m_array[i];
			}
		}
		return NULL;
	}

	void MPreviewController::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		DX11Element2D* element = HitTest(point);
		if (element != m_current)
		{
			m_current = element;
		}
		if (m_current != NULL)
		{
			if (m_current->HitTest(point) >= 0)
			{
				m_current->Track(m_view.Handle(), point, FALSE);
			}
		}
	}

	void MPreviewController::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
	}

	void MPreviewController::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		DX11Element2D* element = HitTest(point);
		if (element != m_current)
		{
			m_current = element;
		}
		if (m_current != NULL)
		{
			m_view.ClientToScreen(&point);
			m_popup.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_view.Handle(), NULL);
			m_bPopup = TRUE;
		}
	}

	void MPreviewController::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_view.Handle();
			tme.dwFlags = TME_LEAVE;
			tme.dwHoverTime = 0;
			m_bTracking = _TrackMouseEvent(&tme);
		}
	}

	void MPreviewController::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_bTracking)
		{
			m_bTracking = FALSE;
		}
		if (!m_bPopup)
		{
			TinyPoint point;
			GetCursorPos(&point);
			::ScreenToClient(m_view.Handle(), &point);
			TinyRectangle rectangle;
			::GetClientRect(m_view.Handle(), &rectangle);
			if (!rectangle.PtInRect(point))
			{
				m_current = NULL;
			}
		}
	}

	void MPreviewController::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point;
		GetCursorPos(&point);
		::ScreenToClient(m_view.Handle(), &point);
		if (m_current = HitTest(point))
		{
			if (m_current->SetCursor(m_view.Handle(), LOWORD(lParam)))
			{
				bHandled = TRUE;
			}
		}
	}

	void MPreviewController::OnMenuClick(void*, INT wID)
	{
		switch (wID)
		{
		case IDM_MOVEDOWN:
			Move(m_current, FALSE);
			break;
		case IDM_MOVEUP:
			Move(m_current, TRUE);
			break;
		case IDM_MOVETOP:
			Bring(m_current, TRUE);
			break;
		case IDM_MOVEBOTTPM:
			Bring(m_current, FALSE);
			break;
		case IDM_REMOVE:
			Remove(m_current);
			break;
		}
		m_bPopup = FALSE;
	}
	MPreviewView& MPreviewController::GetView()
	{
		return m_view;
	}

	DX11RenderView&	MPreviewController::GetRenderView()
	{
		return m_renderView;
	}

	DX11Graphics2D&	MPreviewController::Graphics()
	{
		return m_graphics;
	}
	BOOL MPreviewController::Submit()
	{
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MPreviewController::OnMessagePump, this));
	}

	BOOL MPreviewController::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	BOOL MPreviewController::SetPulgSize(const TinySize& size)
	{
		m_pulgSize = size;
		return m_renderView.Create(static_cast<INT>(m_pulgSize.cx), static_cast<INT>(m_pulgSize.cy), FALSE);
	}

	TinySize MPreviewController::GetPulgSize() const
	{
		return m_pulgSize;
	}

	void MPreviewController::SetVideoController(MVideoController* pCTRL)
	{
		if (pCTRL != NULL)
		{
			for (INT i = 0;i < m_waits.size();i++)
			{
				if (m_waits[i] == pCTRL->m_signal)
				{
					m_index = i;
					break;
				}
			}
		}
		else
		{
			m_index = -1;
		}
	}
	TinyEvent&	MPreviewController::GetSignal()
	{
		return m_signal;
	}
	DWORD MPreviewController::Draw()
	{
		m_timeQPC.BeginTime();
		m_graphics.GetDX11().SetRenderTexture2D(&m_renderView);
		m_graphics.GetDX11().GetRender2D()->BeginDraw();
		TinyArray<DX11Element2D*> images;
		for (INT i = m_array.GetSize() - 1;i >= 0;i--)
		{
			DX11Element2D* ps = m_array[i];
			if (ps->IsKindOf(RUNTIME_CLASS(DX11Image2D)))
			{
				DX11Image2D* image = static_cast<DX11Image2D*>(ps);
				if (ps->IsKindOf(RUNTIME_CLASS(MImageElement)))
				{
					images.Add(image);
				}
				if (ps->IsKindOf(RUNTIME_CLASS(MVideoElement)))
				{
					m_graphics.GetDX11().AllowBlend(FALSE, NULL);
					m_graphics.GetDX11().AllowDepth(TRUE);
					m_graphics.DrawImage(image, (FLOAT)((FLOAT)m_pulgSize.cx / static_cast<FLOAT>(m_graphics.GetDX11().GetSize().cx)), (FLOAT)((FLOAT)m_pulgSize.cy / static_cast<FLOAT>(m_graphics.GetDX11().GetSize().cy)));
				}
			}
		}
		for (INT i = images.GetSize() - 1;i >= 0;i--)
		{
			DX11Element2D* ps = images[i];
			if (ps->IsKindOf(RUNTIME_CLASS(DX11Image2D)))
			{
				DX11Image2D* image = static_cast<DX11Image2D*>(ps);
				FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				m_graphics.GetDX11().AllowBlend(TRUE, blendFactor);
				m_graphics.GetDX11().AllowDepth(FALSE);
				m_graphics.DrawImage(image, (FLOAT)((FLOAT)m_pulgSize.cx / static_cast<FLOAT>(m_graphics.GetDX11().GetSize().cx)), (FLOAT)((FLOAT)m_pulgSize.cy / static_cast<FLOAT>(m_graphics.GetDX11().GetSize().cy)));
			}
		}
		m_graphics.GetDX11().GetRender2D()->EndDraw();
		//////////////////////////////////////////////////////////////////////////
		m_graphics.GetDX11().SetRenderTexture2D(NULL);
		m_graphics.GetDX11().GetRender2D()->BeginDraw();
		INT index = -1;
		for (INT i = m_array.GetSize() - 1;i >= 0;i--)
		{
			DX11Element2D* p2D = m_array[i];
			if (p2D->IsKindOf(RUNTIME_CLASS(DX11Image2D)))
			{
				DX11Image2D* image = static_cast<DX11Image2D*>(p2D);
				if (p2D->IsKindOf(RUNTIME_CLASS(MVideoElement)))
				{
					m_graphics.GetDX11().AllowBlend(FALSE, NULL);
					m_graphics.GetDX11().AllowDepth(TRUE);
					m_graphics.DrawImage(image);
				}
			}
			if (p2D == m_current)
			{
				index = i;
			}
		}
		for (INT i = images.GetSize() - 1;i >= 0;i--)
		{
			DX11Element2D* ps = images[i];
			if (ps->IsKindOf(RUNTIME_CLASS(DX11Image2D)))
			{
				DX11Image2D* image = static_cast<DX11Image2D*>(ps);
				FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				m_graphics.GetDX11().AllowBlend(TRUE, blendFactor);
				m_graphics.GetDX11().AllowDepth(FALSE);
				m_graphics.DrawImage(image);
			}
		}
		if (index >= 0)
		{
			DX11Element2D* p2D = m_array[index];
			if (p2D->IsKindOf(RUNTIME_CLASS(DX11Image2D)))
			{
				FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				m_graphics.GetDX11().AllowBlend(TRUE, blendFactor);
				m_graphics.GetDX11().AllowDepth(FALSE);
				DX11Image2D* image = static_cast<DX11Image2D*>(p2D);
				UINT mask = image->GetHandleMask();
				for (INT i = 0; i < 8; ++i)
				{
					if (mask & (1 << i))
					{
						TinyRectangle rectangle;
						image->GetHandleRect((TrackerHit)i, &rectangle);
						m_handles[i].SetPosition(rectangle.Position());
						m_handles[i].SetScale(rectangle.Size());
						m_graphics.DrawImage(&m_handles[i]);
					}
				}
			}
		}
		m_graphics.GetDX11().GetRender2D()->EndDraw();
		m_graphics.Present();
		m_graphics.Flush();
		m_timeQPC.EndTime();
		return static_cast<DWORD>(m_timeQPC.GetMillisconds());
	}

	void MPreviewController::OnMessagePump()
	{
		for (;;)
		{
			if (m_bBreak)
			{
				for (INT i = 0; i < 8; ++i)
				{
					m_handles[i].Destory();
				}
				for (INT i = 0;i < m_array.GetSize();i++)
				{
					m_array[i]->Deallocate(m_graphics.GetDX11());
				}
				break;
			}
			if (m_waits.size() == 0)
			{
				Sleep(25);
				continue;
			}
			HRESULT hRes = WaitForMultipleObjects(m_waits.size(), &m_waits[0], FALSE, 1000);
			if (hRes == WAIT_FAILED || hRes == WAIT_ABANDONED)
			{
				break;
			}
			TinyAutoLock lock(m_lock);
			this->Draw();
			if ((hRes - WAIT_OBJECT_0) == m_index)
			{
				m_signal.SetEvent();
			}
		}
	}
}
