#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

#include "Setup/BaseSetupDlg.h"
#include "resource.h"

using namespace MySetup;

// CMsgDlg dialog
enum MessageType
{
	MessageUnknown,
	MessageNormal,
	MessageError,
	MessageInfo,
	MessageWarning,
	MessageQuestion,
};

class CMsgDlg :
	public CBaseSetupDlg
{
	DECLARE_DYNCREATE(CMsgDlg)

public:
	CMsgDlg(MessageType msgType = MessageUnknown, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMsgDlg();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_MSG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
private:
	_tstring m_Title;
	_tstring m_Message;
	_tstring m_OK;
	_tstring m_Cancel;
	MessageType m_MessageType;
public:
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	void SetTitle(LPCTSTR title);
	void SetMessage(LPCTSTR msg);
	void SetOkButtonText(LPCTSTR txt);
	void SetCancelButtonText(LPCTSTR txt);	

	static int ShowMessage(
		MessageType msgType,
		CSetupData *pSetupData,
		__in LPCTSTR msg,
		__in_opt LPCTSTR title = NULL);

	static int ShowErrorMessage(
		CSetupData *pSetupData,
		__in LPCTSTR msg,
		__in_opt LPCTSTR title = NULL);



	static int ShowConfirmMessage(
		CSetupData *pSetupData,
		__in LPCTSTR msg,
		__in_opt LPCTSTR title = NULL);
};
