#pragma once
#include <Richedit.h>
#include "../Common/TinyDialog.h"

namespace TinyUI
{
	/// <summary>
	/// �Զ���Ի���
	/// </summary>
	class TinyCustomDialog : public TinyDialog
	{
		DECLARE_DYNAMIC(TinyCustomDialog)
		BEGIN_MSG_MAP(TinyCustomDialog, TinyDialog)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		END_MSG_MAP()
	public:
		TinyCustomDialog();
		virtual ~TinyCustomDialog();
		virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	};
}