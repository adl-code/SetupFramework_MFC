// UI/ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetupProject.h"
#include "UI/ConfigDlg.h"


// CConfigDlg dialog

IMPLEMENT_DYNCREATE(CConfigDlg, CDHtmlDialog)


CConfigDlg::CConfigDlg(CWnd* parent /*= NULL*/)
: CBaseSetupDlg(CConfigDlg::IDD, parent)
{

}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CConfigDlg::OnInitDialog()
{
	CBaseSetupDlg::OnInitDialog();

	CSetupData *pSetupData = GetSetupData();
	ASSERT(pSetupData);

	SetWindowText(pSetupData->GetString("configuration").c_str());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDHtmlDialog)
	ON_WM_CLOSE()	
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CConfigDlg)
	DHTML_EVENT_ONCLICK(_T("button_next"), OnButtonNext)
	DHTML_EVENT_ONCLICK(_T("button_cancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CConfigDlg message handlers

HRESULT CConfigDlg::OnButtonNext(IHTMLElement* /*pElement*/)
{
	SaveConfig();
	EndDialog(SETUP_NEXT);
	return S_OK;
}

HRESULT CConfigDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	EndDialog(SETUP_CANCEL);
	return S_OK;
}

// Load configurations from setup data structure and display it
void CConfigDlg::LoadConfig()
{
	CSetupData *pSetupData = GetSetupData();
	ASSERT(pSetupData);

	std::map<std::string, IHTMLElement*> elements;
	FindConfigElements(elements);
	HRESULT hr;
	for (auto item : elements)
	{
		std::string configName = item.first;
		IHTMLElement *element = item.second;
		std::string configValue;
		
		BSTR tagName;
		if (SUCCEEDED(hr = element->get_tagName(&tagName)) && pSetupData->GetUserConfig(configName.c_str(), configValue))
		{
			if (_tcsicmp(tagName, _T("input")) == 0)
			{
				VARIANT inputType = {};
				VARIANT inputID = {};

				if (SUCCEEDED(hr = element->getAttribute(_T("type"), 0, &inputType)) && inputType.vt == VT_BSTR &&
					SUCCEEDED(hr = element->getAttribute(_T("id"), 0, &inputID)) && inputID.vt == VT_BSTR)
				{
					if (_tcsicmp(inputType.bstrVal, _T("checkbox")) == 0)
					{
						// Parse checkbox bool value
						bool checked = false;
						if (Utils::ParseBoolConfig(configValue.c_str(), checked))
						{
							int checkValue = checked ? 1 : 0;
							DDX_DHtml_CheckBox(inputID.bstrVal, checkValue, FALSE);
						}
					}
				}
			}
		}
		element->Release();
	}
}

// Save user configurations to the internal setup data structure
void CConfigDlg::SaveConfig()
{
	CSetupData *pSetupData = GetSetupData();
	ASSERT(pSetupData);

	std::map<std::string, IHTMLElement*> elements;
	FindConfigElements(elements);
	HRESULT hr;
	for (auto item : elements)
	{
		std::string configName = item.first;
		IHTMLElement *element = item.second;
		std::string configValue;

		BSTR tagName;
		if (SUCCEEDED(hr = element->get_tagName(&tagName)))
		{
			if (_tcsicmp(tagName, _T("input")) == 0)
			{
				VARIANT inputType = {};
				VARIANT inputID = {};

				if (SUCCEEDED(hr = element->getAttribute(_T("type"), 0, &inputType)) && inputType.vt == VT_BSTR &&
					SUCCEEDED(hr = element->getAttribute(_T("id"), 0, &inputID)) && inputID.vt == VT_BSTR)
				{
					if (_tcsicmp(inputType.bstrVal, _T("checkbox")) == 0)
					{
						// Save checkbox value
						int checkValue;
						DDX_DHtml_CheckBox(inputID.bstrVal, checkValue, TRUE);
						pSetupData->SetUserConfig(configName.c_str(), checkValue ? "true" : "false");
					}
				}
			}
		}
		element->Release();
	}
}


void CConfigDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CBaseSetupDlg::OnDocumentComplete(pDisp, szUrl);

	// TODO: Add your specialized code here and/or call the base class
	LoadConfig();
}

// Find all config elements (elements that have values starting with "config_")
void CConfigDlg::FindConfigElements(__inout std::map<std::string, IHTMLElement*> &elementList)
{
	HRESULT hr;
	IHTMLDocument2 *pDoc;
	if (SUCCEEDED(hr = this->GetDHtmlDocument(&pDoc)))
	{
		// Scan all document's elements and get configurations from elements having values starting with "config_"

		// Painful COM manipulations :|
		IHTMLElementCollection *elements;
		if (SUCCEEDED(pDoc->get_all(&elements)))
		{			
			int i = 0;
			while (true)	// Bad practice! It may cause infinite loop, but who cares :D
			{
				VARIANT itemName, itemIdx;
				itemName.vt = itemIdx.vt = VT_I4;
				itemName.intVal = itemIdx.intVal = i++;

				IDispatch *pDispatch;
				if (SUCCEEDED(hr = elements->item(itemName, itemIdx, &pDispatch)) && pDispatch)
				{
					IHTMLElement *pElement;
					if (SUCCEEDED(hr = pDispatch->QueryInterface(IID_IHTMLElement, (LPVOID*)&pElement)))
					{						
						VARIANT elementValue = {};
						if (SUCCEEDED(hr = pElement->getAttribute(_T("value"), 0, &elementValue)) && elementValue.vt == VT_BSTR)
						{
							std::string valueName;
							LPCSTR prefix = "config_";
							size_t prefixLen = strlen(prefix);
							valueName = Utils::MakeLowerA(TSTRING_TO_UTF8(elementValue.bstrVal));
							
							if (_strnicmp(valueName.c_str(), prefix, prefixLen) == 0)
							{
								pElement->AddRef();
								elementList[valueName] = pElement;
							}							
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

		pDoc->Release();
	}
}
