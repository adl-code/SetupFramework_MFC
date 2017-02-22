#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

#include "Setup/BaseSetupDlg.h"

// CMainSetupDlg dialog
using namespace MySetup;

class CMainSetupDlg :
	public CBaseSetupDlg,
	public CSetupObserver
{
	DECLARE_DYNCREATE(CMainSetupDlg)

public:
	CMainSetupDlg(CWnd* parent = NULL);   // standard constructor
	
	virtual ~CMainSetupDlg();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_MAIN_SETUP_DIALOG };

	_tstring m_StatusText;
	_tstring m_ErrorText;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
private:
	void StartSetup();
	void SetSetupProgress(DWORD percentage);
	void SetSetupStatus(LPCTSTR pStatus);
	
public:
	// Implement the CSetupObserver interface
	virtual void UpdateStatus(__in LPCTSTR statusText);

	virtual void UpdateProgress(DWORD percentage);

	virtual void OnError(__in LPCTSTR errorText);

	virtual void OnFinish(int setupResult);

private:
	
	// Handling user defined message to update UI
	LRESULT OnUpdateUI(WPARAM wParam, LPARAM lParam);

	void OnSetupFinish(int setupResult);

	void OnSetupCompleted();
	void OnSetupFailed();

};
