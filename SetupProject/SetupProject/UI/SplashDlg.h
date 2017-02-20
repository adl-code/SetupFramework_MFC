#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

// CSplashDlg dialog
#include "Setup/BaseSetupDlg.h"

using namespace MySetup;

class CSplashDlg :	
	public CBaseSetupDlg
{
	DECLARE_DYNCREATE(CSplashDlg)

public:
	CSplashDlg(CWnd *parent = NULL); // // standard constructor
	CSplashDlg(LPCTSTR htmlResource, CWnd* pParent = NULL);   
	virtual ~CSplashDlg();

// Dialog Data
	enum { IDD = IDD_SPLASH_DIALOG };
private:	
	DWORD m_StartTime;
	DWORD m_SplashTime;
	DWORD m_FadeInTime;
	DWORD m_FadeOutTime;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:		
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
};
