// EulaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetupProject.h"
#include "EulaDlg.h"

#include "Utils/ResourceLoader.h"

// CEulaDlg dialog

IMPLEMENT_DYNCREATE(CEulaDlg, CDHtmlDialog)


CEulaDlg::CEulaDlg(CWnd* parent /*= NULL*/)
: CBaseSetupDlg(CEulaDlg::IDD, parent)
{

}


CEulaDlg::~CEulaDlg()
{
}

void CEulaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CEulaDlg::OnInitDialog()
{
	CBaseSetupDlg::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CEulaDlg, CDHtmlDialog)
	ON_WM_CLOSE()	
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CEulaDlg)
	DHTML_EVENT_ONCLICK(_T("button_next"), OnButtonNext)
	DHTML_EVENT_ONCLICK(_T("button_cancel"), OnButtonCancel)
	DHTML_EVENT_ONCLICK(_T("checkbox_accept"), OnCheckAccept)
END_DHTML_EVENT_MAP()



// CEulaDlg message handlers

HRESULT CEulaDlg::OnButtonNext(IHTMLElement* pElement)
{
	if (!CBaseSetupDlg::IsElementDisabled(pElement))
		EndDialog(SETUP_OK);
	return S_OK;
}

HRESULT CEulaDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	CSetupData *pSetupData = GetSetupData();
	ASSERT(pSetupData);

	if (pSetupData->CouldBeBack())
		EndDialog(SETUP_BACK);
	else
		EndDialog(SETUP_CANCEL);
	return S_OK;
}

HRESULT CEulaDlg::OnCheckAccept(IHTMLElement* /*pElement*/)
{
	int checkBoxValue = 0;
	DDX_DHtml_CheckBox(_T("checkbox_accept"), checkBoxValue, TRUE);
	VARIANT allowNextButton;
	allowNextButton.vt = VT_BOOL;
	allowNextButton.boolVal = checkBoxValue ? VARIANT_FALSE : VARIANT_TRUE;
	
	IHTMLElement *button;
	if (SUCCEEDED(GetElement(_T("button_next"), &button)))
	{
		button->setAttribute(_T("disabled"), allowNextButton);
		button->Release();
	}
	return S_OK;
}



void CEulaDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	__super::OnDocumentComplete(pDisp, szUrl);

	// TODO: Add your specialized code here and/or call the base class

	// Load the EULA content
	CSetupData *pSetupData = GetSetupData();
	ASSERT(pSetupData);

	_tstring eula = pSetupData->GetString("eula_content");
		
	if (!eula.empty())
	{	
		CString eulaContent = eula.c_str();
		
		DDX_DHtml_ElementText(_T("eula"), DISPID_IHTMLELEMENT_INNERHTML, eulaContent, FALSE);
		
	}
}

