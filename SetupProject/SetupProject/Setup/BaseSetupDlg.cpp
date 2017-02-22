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
	if (pDoc == NULL || pSetupData == NULL) return;

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

					hr = pElement->put_innerHTML((BSTR)TSTRING_TO_UNICODE(pSetupData->GetString(stringID.c_str())).c_str());
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

int MySetup::CBaseSetupDlg::Display(
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
	std::string configValue;
	LONG width, height;
	LPCSTR screenId = m_ScreenId.c_str();
	if (m_SetupData->GetScreenConfig(screenId, "width", configValue) && Utils::ParseLongConfig(configValue.c_str(), width) &&
		m_SetupData->GetScreenConfig(screenId, "height", configValue) && Utils::ParseLongConfig(configValue.c_str(), height) &&
		width > 0 && height > 0)
	{
		// Update screen width and height
		HMONITOR currentMonitor = MonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTONEAREST);
		if (currentMonitor)
		{
			MONITORINFO monitor = {};
			monitor.cbSize = sizeof(monitor);
			if (GetMonitorInfo(currentMonitor, &monitor))
			{
				LONG cx = monitor.rcWork.right - monitor.rcWork.left;
				LONG cy = monitor.rcWork.bottom - monitor.rcWork.top;

				LONG x = monitor.rcWork.left + (cx - width) / 2;
				LONG y = monitor.rcWork.top + (cy - height) / 2;

				SetWindowPos(NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}	
	}

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
	bool shouldHideBorder = false;
	bool shouldBeTopMost = false;

	if (m_SetupData->GetScreenConfig(screenId, "no_border", configValue))
		Utils::ParseBoolConfig(configValue.c_str(), shouldHideBorder);

	if (m_SetupData->GetScreenConfig(screenId, "top_most", configValue))
		Utils::ParseBoolConfig(configValue.c_str(), shouldBeTopMost);

	if (shouldHideBorder)
	{
		ModifyStyle(WS_CAPTION | WS_BORDER | WS_THICKFRAME, 0, SWP_FRAMECHANGED);
		ModifyStyleEx(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
		
	}

	if (shouldBeTopMost)
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

	if (m_SetupData->CouldBeBack())
	{
		// Modify the "Cancel" button text to "Back"
		
		_tstring backText = m_SetupData->GetString("back");
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
	UNREFERENCED_PARAMETER(dwID);
	UNREFERENCED_PARAMETER(ppt);
	UNREFERENCED_PARAMETER(pcmdtReserved);
	UNREFERENCED_PARAMETER(pdispReserved);
	// Just do nothing to prevent displaying context-menu
	return S_OK;
}

bool MySetup::CBaseSetupDlg::IsElementDisabled(IHTMLElement *pElement)
{
	VARIANT disabled = {};
	if (pElement  && SUCCEEDED(pElement->getAttribute(_T("disabled"), 0, &disabled))
		&& disabled.vt == VT_BOOL && disabled.boolVal == VARIANT_TRUE)
		return true;
	return false;
}
