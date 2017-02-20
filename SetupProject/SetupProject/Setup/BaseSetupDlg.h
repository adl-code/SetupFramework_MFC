#pragma once

#include "Setup/SetupData.h"
#include "Utils/StringUtils.h"

namespace MySetup
{
	class CBaseSetupDlg:
		public CDHtmlDialog
	{
	public:
		CBaseSetupDlg(UINT dlgID, LPCTSTR htmlResource, CWnd *parent);
		virtual ~CBaseSetupDlg() {}
		

	private:
		// Class data
		CSetupData *m_SetupData;	
	protected:
		// Provide methods for children to access internal data
		CSetupData *GetSetupData();
	public:
		int DoSetup(CSetupData *pSetupData);

		DECLARE_MESSAGE_MAP();
		
	private:
		// Internal methods
		void UpdateElementText(
			__in IHTMLDocument2 *pDoc,
			__in CSetupData *pSetupData);

		// Event handling
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual void PreInitDialog();
		virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
		HRESULT WINAPI ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
		HRESULT WINAPI GetHostInfo(DOCHOSTUIINFO *pInfo);
		virtual void OnClose();				
	};
}
