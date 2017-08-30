#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualVScrollBar;
		/// <summary>
		/// ��ֱ����
		/// </summary>
		class TinyVisualVBoxLayout : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualVBoxLayout)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualVBoxLayout)
		protected:
			TinyVisualVBoxLayout(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualVBoxLayout();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL IsLayout() const OVERRIDE;
		public:
			void AutoScroll(BOOL bAuto);
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnMouseEnter() OVERRIDE;
			HRESULT	OnMouseLeave() OVERRIDE;
			HRESULT	OnInitialize() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			virtual void OnPosChange(BOOL, INT, INT, INT);
		private:
			void AdjustLayout(TinyVisual* spvis, INT dx, INT dy);
		private:
			BOOL					m_bAuto;
			TinyVisualVScrollBar*	m_scrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}



