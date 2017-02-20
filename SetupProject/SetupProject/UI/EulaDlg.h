#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

#include "Setup/BaseSetupDlg.h"

using namespace MySetup;

// CEulaDlg dialog

class CEulaDlg :	
	public CBaseSetupDlg
{
	DECLARE_DYNCREATE(CEulaDlg)

public:
	CEulaDlg(CWnd* pParent = NULL);   // standard constructor	
	virtual ~CEulaDlg();
// Overrides
	HRESULT OnButtonNext(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
	HRESULT OnCheckAccept(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_EULA_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	
};
