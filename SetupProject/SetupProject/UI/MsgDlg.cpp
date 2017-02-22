// UI/MsgDlg.cpp : implementation file
//

#include "stdafx.h"

#include "UI/MsgDlg.h"
#include "Setup/ConfigData.h"


// CMsgDlg dialog

IMPLEMENT_DYNCREATE(CMsgDlg, CDHtmlDialog)

CMsgDlg::CMsgDlg(MessageType msgType /*= MessageUnknown*/, CWnd* pParent /*=NULL*/)
: CBaseSetupDlg(CMsgDlg::IDD, pParent)
, m_MessageType(msgType)
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CMsgDlg::OnInitDialog()
{
	CBaseSetupDlg::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CMsgDlg, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CMsgDlg)
	DHTML_EVENT_ONCLICK(_T("button_ok"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("button_cancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CMsgDlg message handlers

HRESULT CMsgDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CMsgDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}


void CMsgDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CBaseSetupDlg::OnDocumentComplete(pDisp, szUrl);

	// TODO: Add your specialized code here and/or call the base class

	// Update message, title, button captions
	CString text = m_Title.c_str();
	DDX_DHtml_ElementText(_T("title"), DISPID_IHTMLELEMENT_INNERHTML, text, FALSE);

	text = m_Message.c_str();
	DDX_DHtml_ElementText(_T("message"), DISPID_IHTMLELEMENT_INNERHTML, text, FALSE);

	text = m_OK.c_str();
	DDX_DHtml_ElementText(_T("button_ok"), DISPID_IHTMLELEMENT_INNERHTML, text, FALSE);

	text = m_Cancel.c_str();
	DDX_DHtml_ElementText(_T("button_cancel"), DISPID_IHTMLELEMENT_INNERHTML, text, FALSE);

	// Update icon
	IHTMLElement *pElement = NULL;
	if (SUCCEEDED(GetElement(_T("message_icon"), &pElement)) && pElement)
	{
		BSTR className = NULL;
		switch (m_MessageType)
		{
		case MessageWarning:
			className = SysAllocString(_T("msg_icon_warning"));
			break;
		case MessageError:
			className = SysAllocString(_T("msg_icon_error"));
			break;
		case MessageInfo:
			className = SysAllocString(_T("msg_icon_info"));
			break;
		case MessageQuestion:
			className = SysAllocString(_T("msg_icon_question"));
			break;
		}

		if (className)
		{			
			pElement->put_className(className);
			SysFreeString(className);
		}
		pElement->Release();
	}
}

void CMsgDlg::SetTitle(LPCTSTR title)
{
	if (title) m_Title = title;
}

void CMsgDlg::SetMessage(LPCTSTR msg)
{
	if (msg) m_Message = msg;
}

void CMsgDlg::SetOkButtonText(LPCTSTR txt)
{
	if (txt) m_OK = txt;
}

void CMsgDlg::SetCancelButtonText(LPCTSTR txt)
{
	if (txt) m_Cancel = txt;
}


int CMsgDlg::ShowMessage(
	MessageType msgType,
	CSetupData *pSetupData,
	__in LPCTSTR msg,
	__in_opt LPCTSTR title /*= NULL*/)
{	
	if (pSetupData == NULL) return IDCANCEL;

	CMsgDlg *msgDlg = new CMsgDlg(msgType);

	if (msg) msgDlg->SetMessage(msg);

	if (title)
		msgDlg->SetTitle(title);
	else
		msgDlg->SetTitle(pSetupData->GetString(SID_APP_TITLE).c_str());

	msgDlg->SetOkButtonText(pSetupData->GetString("ok").c_str());
	msgDlg->Display(pSetupData, SETUP_SCREEN_ID_MESSAGE);
	delete msgDlg;
	return IDOK;
}

int CMsgDlg::ShowErrorMessage(
	__in CSetupData *pSetupData,
	__in LPCTSTR msg,
	__in_opt LPCTSTR title /*= NULL*/)
{
	return CMsgDlg::ShowMessage(MessageError, pSetupData, msg, title);
}

int CMsgDlg::ShowConfirmMessage(
	CSetupData *pSetupData,
	__in LPCTSTR msg,
	__in_opt LPCTSTR title /*= NULL*/)
{
	if (pSetupData == NULL) return IDCANCEL;

	CMsgDlg *msgDlg = new CMsgDlg(MessageQuestion);
	
	if (msg) msgDlg->SetMessage(msg);

	if (title)
		msgDlg->SetTitle(title);
	else
		msgDlg->SetTitle(pSetupData->GetString(SID_APP_TITLE).c_str());

	msgDlg->SetOkButtonText(pSetupData->GetString("yes").c_str());
	msgDlg->SetCancelButtonText(pSetupData->GetString("no").c_str());
	int result = msgDlg->Display(pSetupData, SETUP_SCREEN_ID_CONFIRM);
	delete msgDlg;
	return (result == IDYES || result == IDOK) ? IDYES : IDNO;
}
