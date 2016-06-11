#include "../stdafx.h"
#include "TinyVisual.h"
#include "TinyVisualTree.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisual::TinyVisual(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:m_spvisParent(spvisParent),
			m_spvisNext(NULL),
			m_spvisChild(NULL),
			m_spvisOwner(NULL),
			m_hrgnClip(NULL),
			m_vtree(vtree),
			m_visible(TRUE),
			m_enable(TRUE)
		{

		}
		TinyVisual::~TinyVisual()
		{

		}
		void TinyVisual::SetText(LPCSTR pzText)
		{
			m_strText = pzText;
		}
		TinyString	TinyVisual::GetText() const
		{
			return m_strText;
		}
		void TinyVisual::SetName(LPCSTR pzName)
		{
			m_strName = pzName;
		}
		TinyString	TinyVisual::GetName() const
		{
			return m_strName;
		}
		void TinyVisual::SetToolTip(LPCSTR pzTitle)
		{
			m_strToolTip = pzTitle;
		}
		TinyString	TinyVisual::GetToolTip() const
		{
			return m_strToolTip;
		}
		void TinyVisual::SetMaximumSize(const TinySize& size)
		{
			if (m_maximumSize != size)
			{
				m_maximumSize = size;
			}
		}
		void TinyVisual::SetMinimumSize(const TinySize& size)
		{
			if (m_minimumSize != size)
			{
				m_minimumSize = size;
			}
		}
		TinySize TinyVisual::GetMaximumSize() const
		{
			return m_maximumSize;
		}
		TinySize TinyVisual::GetMinimumSize() const
		{
			return m_minimumSize;
		}
		BOOL TinyVisual::IsVisible() const
		{
			return m_visible;
		}
		BOOL TinyVisual::IsEnable() const
		{
			return m_enable;
		}
		void TinyVisual::SetVisible(BOOL visible)
		{
			if (m_visible != visible)
			{
				m_visible = visible;
				for (TinyVisual* pv = this->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					if (!pv) continue;;
					pv->SetVisible(visible);
				}
			}
		}
		void TinyVisual::SetEnable(BOOL enable)
		{
			if (m_enable != enable)
			{
				m_enable = enable;
				for (TinyVisual* pv = this->m_spvisChild; pv != NULL; pv = pv->m_spvisNext)
				{
					if (!pv) continue;;
					pv->SetEnable(enable);
				}
			}
		}
		TinyPoint TinyVisual::GetPosition() const
		{
			return *((TinyPoint*)&m_windowRect);
		}
		void TinyVisual::SetPosition(const TinyPoint& pos)
		{
			TinyPoint& ps = *((TinyPoint*)&m_windowRect);
			ps.x = pos.x;
			ps.y = pos.y;
		}
		TinySize TinyVisual::GetSize() const
		{
			return m_windowRect.Size();
		}
		void TinyVisual::SetSize(const TinySize& size)
		{
			TinySize oldSize = *((TinySize*)&m_windowRect + 1);
			if (oldSize != size)
			{
				m_windowRect.SetSize(size);
				OnSizeChange(oldSize, size);
			}
		}
		void TinyVisual::SetClip(HRGN hrgnClip)
		{
			this->m_hrgnClip = hrgnClip;
		}
		BOOL TinyVisual::SetStyleImage(StyleImage type, LPCSTR pzFile)
		{
			return m_images[(INT)type].Load(pzFile);
		}
		BOOL TinyVisual::SetStyleImage(StyleImage type, BYTE*	ps, DWORD dwSize)
		{
			return m_images[(INT)type].Load(ps, dwSize);
		}
		void TinyVisual::OnSizeChange(const TinySize&oldSize, const TinySize&newSize)
		{

		}
		void TinyVisual::Resize()
		{

		}
	}
}
