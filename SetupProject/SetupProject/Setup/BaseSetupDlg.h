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
		int Display(
			__in CSetupData *pSetupData,
			__in LPCSTR screenId);

		DECLARE_MESSAGE_MAP();
		DECLARE_DHTML_EVENT_MAP();
	private:
		// Internal data
		std::string m_ScreenId;

		// Internal methods
		void UpdateElementText(
			__in IHTMLDocument2 *pDoc,
			__in CSetupData *pSetupData);

		POINT m_MyMouse;
		bool m_IsMoving;
	public:
		// Event handling
	
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual void PreInitDialog();
		virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
		virtual void OnClose();		
		virtual BOOL OnInitDialog();
		
		virtual HRESULT WINAPI ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);

		static bool IsElementDisabled(IHTMLElement *pElement);	

	protected:
		// Make available for subclassed
		// Those three methods provide capability of moving the dialog using mouse dragging gesture
		HRESULT OnBodyMouseDown(IHTMLElement *pElement);
		HRESULT OnBodyMouseUp(IHTMLElement *pElement);
		HRESULT OnBodyMouseMove(IHTMLElement *pElement);
	};
}
