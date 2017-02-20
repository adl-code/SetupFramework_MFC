#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

#include "Setup/BaseSetupDlg.h"

using namespace MySetup;

// CConfigDlg dialog

class CConfigDlg :	
	public CBaseSetupDlg

{
	DECLARE_DYNCREATE(CConfigDlg)

public:
	CConfigDlg(CWnd* parent = NULL); // standard constructor
	
	virtual ~CConfigDlg();
// Overrides
	HRESULT OnButtonNext(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);	

// Dialog Data
	enum { IDD = IDD_CONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
private:
	// Load configurations from the internal setup data structure and display it
	void LoadConfig();

	// Save user configurations to the internal setup data structure
	void SaveConfig();

public:
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);

private:
	// Find all config elements (elements that have values starting with "config_")
	void FindConfigElements(
		__inout std::map<std::string, IHTMLElement*> &elementList);
};
