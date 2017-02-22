// UI/MainSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetupProject.h"

#include "UI/MainSetupDlg.h"
#include "UI/MsgDlg.h"

#include "Setup/ConfigData.h"

// Using user defined message to update UI
// WPARAM: UI component to update
// LPARAM: other param

#define UPDATE_STATUS_TEXT	1
#define UPDATE_ERROR_TEXT	2
#define UPDATE_PROGRESS		3		// LPARAM: percentage
#define UPDATE_FINISH		4		// WPARAM: result

#define WM_UPDATE_UI		(WM_USER + 10)

// CMainSetupDlg dialog

IMPLEMENT_DYNCREATE(CMainSetupDlg, CDHtmlDialog)

CMainSetupDlg::CMainSetupDlg(CWnd* parent /*=NULL*/)
: CBaseSetupDlg(CMainSetupDlg::IDD, parent)
{

}


CMainSetupDlg::~CMainSetupDlg()
{
}

void CMainSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CMainSetupDlg::OnInitDialog()
{
	CBaseSetupDlg::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CMainSetupDlg, CDHtmlDialog)
	ON_MESSAGE(WM_UPDATE_UI, &CMainSetupDlg::OnUpdateUI)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CMainSetupDlg)
	DHTML_EVENT_ONCLICK(_T("button_next"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("button_cancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CMainSetupDlg message handlers

HRESULT CMainSetupDlg::OnButtonOK(IHTMLElement* pElement)
{
	if (!CBaseSetupDlg::IsElementDisabled(pElement))
		EndDialog(SETUP_OK);
	return S_OK;
}

HRESULT CMainSetupDlg::OnButtonCancel(IHTMLElement* pElement)
{
	if (!CBaseSetupDlg::IsElementDisabled(pElement))
	{
		CSetupData *setupData = CBaseSetupDlg::GetSetupData();
		ASSERT(setupData);

		setupData->PauseSetup();
		int result = CMsgDlg::ShowConfirmMessage(setupData, setupData->GetString(SID_CONFIRM_EXIT).c_str());
			setupData->ResumeSetup();
		if (result == IDYES)
			setupData->Stop();
	}
	return S_OK;
}


void CMainSetupDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CBaseSetupDlg::OnDocumentComplete(pDisp, szUrl);

	// TODO: Add your specialized code here and/or call the base class
	if (szUrl && _tcslen(szUrl) > 0)
		StartSetup();
}

void CMainSetupDlg::StartSetup()
{

}

void CMainSetupDlg::UpdateStatus(__in LPCTSTR statusText)
{
	if (statusText && IsWindow(this->GetSafeHwnd()))
	{
		m_StatusText = statusText;
		PostMessage(WM_UPDATE_UI, UPDATE_STATUS_TEXT);
	}
}

void CMainSetupDlg::UpdateProgress(DWORD percentage)
{
	if (!IsWindow(this->GetSafeHwnd())) return;
	PostMessage(WM_UPDATE_UI, UPDATE_PROGRESS, (LPARAM)percentage);
}

void CMainSetupDlg::OnError(__in LPCTSTR errorText)
{
	if (errorText && IsWindow(this->GetSafeHwnd()))
	{
		m_ErrorText = errorText;
		PostMessage(WM_UPDATE_UI, UPDATE_ERROR_TEXT);
	}
}

void CMainSetupDlg::OnFinish(int setupResult)
{
	if (IsWindow(this->GetSafeHwnd()))
		PostMessage(WM_UPDATE_UI, UPDATE_FINISH, (LPARAM)setupResult);
}

void CMainSetupDlg::SetSetupProgress(DWORD percentage)
{
	CString widthText;
	widthText.Format(_T("%d%%"), percentage > 100 ? 100 : percentage);

	VARIANT withProp;
	withProp.vt = VT_BSTR;
	withProp.bstrVal = widthText.GetBuffer();

	HRESULT hr;
	IHTMLElement *pStatusBar = NULL;
	if (SUCCEEDED(hr = GetElementInterface(_T("the_progress_bar"), __uuidof(pStatusBar), (void**)&pStatusBar) && pStatusBar))
	{
		IHTMLStyle *pStyle = NULL;
		
		if (SUCCEEDED(hr = pStatusBar->get_style(&pStyle)) && pStyle)		
		{	
			hr = pStyle->setAttribute(_T("width"), withProp);
			pStyle->Release();
		}
	
		pStatusBar->Release();
	}	
}


void CMainSetupDlg::SetSetupStatus(LPCTSTR pStatus)
{
	if (pStatus == NULL) return;

	CString status = pStatus;
	DDX_DHtml_ElementText(_T("status"), DISPID_IHTMLELEMENT_INNERHTML, status, FALSE);
}


// Handling user defined message to update UI
LRESULT CMainSetupDlg::OnUpdateUI(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case UPDATE_STATUS_TEXT:
		SetSetupStatus(m_StatusText.c_str());
		break;
	case UPDATE_ERROR_TEXT:
		break;
	case UPDATE_PROGRESS:
		SetSetupProgress((DWORD)lParam);
		break;
	case UPDATE_FINISH:
		OnSetupFinish((int)lParam);
		break;
	}
	return 0;
}

void CMainSetupDlg::OnSetupFinish(int setupResult)
{
	switch (setupResult)
	{
	case SETUP_OK:
		OnSetupCompleted();
		break;
	case SETUP_ERROR:
		OnSetupFailed(); // Display error message
		EndDialog(setupResult);
		break;
	default:
		EndDialog(setupResult);
		break;
	}
}

void CMainSetupDlg::OnSetupCompleted()
{
	IHTMLElement *button = NULL;
	VARIANT val;
	val.vt = VT_BOOL;
	val.boolVal = VARIANT_FALSE;

	if (SUCCEEDED(GetElement(_T("button_next"), &button)))
	{
		button->setAttribute(_T("disabled"), val);
		button->Release();
	}

	val.vt = VT_BOOL;
	val.boolVal = VARIANT_TRUE;

	if (SUCCEEDED(GetElement(_T("button_cancel"), &button)))
	{
		button->setAttribute(_T("disabled"), val);
		button->Release();
	}

	CSetupData *pSetupData = CBaseSetupDlg::GetSetupData();
	ASSERT(pSetupData);
	CString status = pSetupData->GetString("setup_completed").c_str();
	DDX_DHtml_ElementText(_T("status"), DISPID_IHTMLELEMENT_INNERHTML, status, FALSE);

}

void CMainSetupDlg::OnSetupFailed()
{
	CSetupData *pSetupData = CBaseSetupDlg::GetSetupData();
	ASSERT(pSetupData);
	_tstring msg = pSetupData->GetString("setup_failed");
	CMsgDlg::ShowErrorMessage(pSetupData, msg.c_str());
}
