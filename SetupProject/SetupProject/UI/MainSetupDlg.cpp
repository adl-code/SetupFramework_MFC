// UI/MainSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetupProject.h"
#include "UI/MainSetupDlg.h"


// CMainSetupDlg dialog

IMPLEMENT_DYNCREATE(CMainSetupDlg, CDHtmlDialog)

CMainSetupDlg::CMainSetupDlg(CWnd* parent /*=NULL*/)
: CBaseSetupDlg(CMainSetupDlg::IDD, _T("MainSetupDlg"), parent)
{

}

CMainSetupDlg::CMainSetupDlg(LPCTSTR htmlResource, CWnd *parent /*= NULL*/)
: CBaseSetupDlg(CMainSetupDlg::IDD, htmlResource, parent)
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
	CDHtmlDialog::OnInitDialog();
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
