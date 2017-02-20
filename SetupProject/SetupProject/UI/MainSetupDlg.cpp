// UI/MainSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetupProject.h"
#include "UI/MainSetupDlg.h"


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
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CMainSetupDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CMainSetupDlg message handlers

HRESULT CMainSetupDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	EndDialog(SETUP_OK);
	return S_OK;
}

HRESULT CMainSetupDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	EndDialog(SETUP_CANCEL);
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
	CSetupData *pSetupData = GetSetupData();
	ASSERT(pSetupData);

	CString statusText;

#if defined ONLINE_INSTALLER
	statusText = pSetupData->GetStringLoader()->LoadString("downloading_installer").c_str();
#elif defined OFFLINE_INSTALLER
	statusText = pSetupData->GetStringLoader()->LoadString("extracting_installer").c_str();
#endif

	DDX_DHtml_ElementText(_T("status"), DISPID_IHTMLELEMENT_INNERHTML, statusText, FALSE);
}
