#include "stdafx.h"
#include "BaseSetupDlg.h"


BEGIN_MESSAGE_MAP(MySetup::CBaseSetupDlg, CDHtmlDialog)
	
END_MESSAGE_MAP()

MySetup::CBaseSetupDlg::CBaseSetupDlg(UINT dlgID, CWnd *parent)
: CDHtmlDialog(MAKEINTRESOURCE(dlgID), NULL, parent)
, m_SetupData(NULL)
{

}

void MySetup::CBaseSetupDlg::UpdateElementText(
	__in IHTMLDocument2 *pDoc,
	__in CSetupData *pSetupData)
{
	// Update text for any HTML control that has the "text_id" attribute
	if (pDoc == NULL || pSetupData == NULL || pSetupData->GetStringLoader() == NULL) return;

	HRESULT hr;

	IHTMLElementCollection *elements;
	if (!SUCCEEDED(hr = pDoc->get_all(&elements)))
		return;


	int i = 0;
	while (true)	// Bad practice!
	{
		VARIANT idx;
		VARIANT name;
		idx.vt = VT_I4;
		idx.intVal = i;

		name.vt = VT_I4;
		name.intVal = i;

		IDispatch *pDispatch = NULL;
		i++;
		if (SUCCEEDED(hr = elements->item(name, idx, &pDispatch))
			&& pDispatch /* MS sucks! In case there is no more item to enumerate,
						 IHTMLElementCollection.item still returns S_OK but pDispatch is NULL! */)
		{
			IHTMLElement *pElement;
			if (SUCCEEDED(hr = pDispatch->QueryInterface(IID_IHTMLElement, (LPVOID*)&pElement)))
			{				
				VARIANT textID;
				if (SUCCEEDED(hr = pElement->getAttribute (_T("text_id"), 0, &textID)) && textID.vt == VT_BSTR)
				{
					std::string stringID;
					stringID = Utils::UnicodeToUtf8(textID.bstrVal);

					hr = pElement->put_innerHTML((BSTR)TSTRING_TO_UNICODE(pSetupData->GetStringLoader()->LoadString(stringID.c_str(), pSetupData->GetLanguageID().c_str())).c_str());
				}
				pElement->Release();
			}
			pDispatch->Release();
		}
		else			
			break;
	}
	

	elements->Release();
}

MySetup::CSetupData* MySetup::CBaseSetupDlg::GetSetupData()
{
	return m_SetupData;
}

int MySetup::CBaseSetupDlg::DoSetup(
	__in CSetupData *pSetupData,
	__in LPCSTR screenId)
{
	if (screenId == NULL || pSetupData == NULL) return SETUP_ERROR;

	m_ScreenId = screenId;
	
	m_SetupData = pSetupData;
	return DoModal();
}


BOOL MySetup::CBaseSetupDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if ((pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP ||pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP) &&
		(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
		pMsg->wParam == VK_F5 || pMsg->wParam == VK_F11 || pMsg->wParam == VK_F12 ||
		pMsg->wParam == VK_CONTROL))
		return TRUE;
	return CDHtmlDialog::PreTranslateMessage(pMsg);
}


void MySetup::CBaseSetupDlg::PreInitDialog()
{
	// TODO: Add your specialized code here and/or call the base class

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
	if (m_SetupData->ShouldHideBorder())
	{
		ModifyStyle(WS_CAPTION, 0);
		ModifyStyleEx(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
		
	}

	if (m_SetupData->ShouldBeTopMost())
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	CDHtmlDialog::PreInitDialog();
}


void MySetup::CBaseSetupDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CDHtmlDialog::OnDocumentComplete(pDisp, szUrl);

	// TODO: Add your specialized code here and/or call the base class
	if (szUrl == NULL || _tcslen(szUrl) == 0) return;

	// Reload control text from text resource
	HRESULT hr;
	IHTMLDocument2 *pDoc;
	if (SUCCEEDED(hr = this->GetDHtmlDocument(&pDoc)))
	{
		UpdateElementText(pDoc, m_SetupData);

		pDoc->Release();
	}

	if (m_SetupData->CouldBeBack() && m_SetupData->GetStringLoader())
	{
		// Modify the "Cancel" button text to "Back"
		
		_tstring backText = m_SetupData->GetStringLoader()->LoadString("back", m_SetupData->GetLanguageID().c_str());
		if (!backText.empty())
		{
			CString backButton = backText.c_str();
			DDX_DHtml_ElementText(_T("button_cancel"), DISPID_IHTMLELEMENT_INNERHTML, backButton, FALSE);
		}

		
	}
}

void MySetup::CBaseSetupDlg::OnClose()
{
	// Just do nothing to prevent the window to be closed by pressing ALT-F4

}


BOOL MySetup::CBaseSetupDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// TODO: Add your message handler code here
	std::string resourceID;
	if (!m_SetupData->GetScreenConfig(m_ScreenId.c_str(), "resource", resourceID)) return SETUP_ERROR;
		LoadFromResource(UTF8_TO_TSTRING(resourceID).c_str());

	return TRUE;
}

HRESULT WINAPI MySetup::CBaseSetupDlg::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
#if defined _DEBUG || defined DEBUG
	return __super::ShowContextMenu(dwID, ppt, pcmdtReserved, pdispReserved);
#else

	// Just do nothing to prevent displaying context-menu
	return S_OK;
#endif
}
