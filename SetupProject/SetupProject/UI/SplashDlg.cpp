// UI/SplashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetupProject.h"
#include "UI/SplashDlg.h"
#include "afxdialogex.h"


// CSplashDlg dialog
#define SPLASH_TIMER_ID			100
#define SPLASH_TIMER_INTERVAL	10		// 10 milliseconds

IMPLEMENT_DYNCREATE(CSplashDlg, CDHtmlDialog)

CSplashDlg::CSplashDlg(CWnd *parent /*= NULL*/)
: CBaseSetupDlg(CSplashDlg::IDD, _T("SPLASH_DLG"), parent)
, m_StartTime(0)
, m_FadeInTime(0)
, m_FadeOutTime(0)
, m_SplashTime(0)
{

}

CSplashDlg::CSplashDlg(LPCTSTR htmlResource, CWnd* parent /*=NULL*/)
: CBaseSetupDlg(CSplashDlg::IDD, htmlResource, parent)
, m_StartTime(0)
, m_FadeInTime(0)
, m_FadeOutTime(0)
, m_SplashTime(0)
{

}

CSplashDlg::~CSplashDlg()
{
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CSplashDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	// Make myself topmost
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	CSetupData *pSetupData = GetSetupData();
	// Start the timer
	ASSERT(pSetupData);
	
	// Parsing dialog configuration
	std::string configValue;
	unsigned long ulongValue;

	if (pSetupData->GetScreenConfig("SPLASH", "splash_time", configValue) &&
		Utils::ParseULongConfig(configValue.c_str(), ulongValue))
		m_SplashTime = ulongValue;

	if (pSetupData->GetScreenConfig("SPLASH", "fade_in_time", configValue) &&
		Utils::ParseULongConfig(configValue.c_str(), ulongValue))
		m_FadeInTime = ulongValue;

	if (pSetupData->GetScreenConfig("SPLASH", "fade_out_time", configValue) &&
		Utils::ParseULongConfig(configValue.c_str(), ulongValue))
		m_FadeOutTime = ulongValue;

	m_StartTime = GetTickCount();
	
	if (m_FadeInTime || m_FadeOutTime)
		ModifyStyleEx(0, WS_EX_LAYERED);
	
	SetTimer(SPLASH_TIMER_ID, SPLASH_TIMER_INTERVAL, NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CSplashDlg, CDHtmlDialog)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CSplashDlg)

END_DHTML_EVENT_MAP()



// CSplashDlg message handlers

void CSplashDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	
	if (nIDEvent == SPLASH_TIMER_ID)
	{
		DWORD elapsedTime = GetTickCount() - m_StartTime;
		
		if (elapsedTime < m_FadeInTime)
			__super::SetLayeredWindowAttributes(0, (BYTE)(elapsedTime * 255 / m_FadeInTime), LWA_ALPHA);
		else if (elapsedTime >= m_FadeInTime &&
			elapsedTime < (m_FadeInTime + m_SplashTime))
		{
			if (m_FadeInTime)
				SetLayeredWindowAttributes(0, 255, LWA_ALPHA);
		}
		else if (elapsedTime < (m_FadeInTime + m_SplashTime + m_FadeOutTime))
		{
			DWORD transparency = (elapsedTime - m_FadeInTime - m_SplashTime) * 255 / m_FadeOutTime;
			if (transparency > 0)
				SetLayeredWindowAttributes(0, 255 - (BYTE)transparency, LWA_ALPHA);
		}
		else
			EndDialog(SETUP_OK);
	}
	__super::OnTimer(nIDEvent);
}