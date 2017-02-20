#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

#include "Setup/BaseSetupDlg.h"

// CMainSetupDlg dialog
using namespace MySetup;

class CMainSetupDlg :
	public CBaseSetupDlg
{
	DECLARE_DYNCREATE(CMainSetupDlg)

public:
	CMainSetupDlg(CWnd* parent = NULL);   // standard constructor
	CMainSetupDlg(LPCTSTR htmlResource, CWnd *parent = NULL);
	virtual ~CMainSetupDlg();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_MAIN_SETUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
