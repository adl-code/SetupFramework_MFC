#pragma once

#include "Setup/SetupData.h"
#include "Utils/StringUtils.h"

namespace MySetup
{
	class CBaseSetupDlg:
		public CDHtmlDialog
	{
	public:
		CBaseSetupDlg(UINT dlgID, CWnd *parent);	
		virtual ~CBaseSetupDlg() {}
		

	private:
		// Class data
		CSetupData *m_SetupData;	
	protected:
		// Provide methods for children to access internal data
		CSetupData *GetSetupData();
	public:
		int DoSetup(
			__in CSetupData *pSetupData,
			__in LPCSTR screenId);

		DECLARE_MESSAGE_MAP();
		
	private:
		// Internal data
		std::string m_ScreenId;

		// Internal methods
		void UpdateElementText(
			__in IHTMLDocument2 *pDoc,
			__in CSetupData *pSetupData);
	public:
		// Event handling
	
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual void PreInitDialog();
		virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);		
		virtual void OnClose();				
		virtual BOOL OnInitDialog();
		virtual HRESULT WINAPI ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	};
}
