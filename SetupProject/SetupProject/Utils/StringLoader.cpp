#include "stdafx.h"
#include "Utils/StringLoader.h"
#include "Utils/ResourceLoader.h"
#include <algorithm>

Utils::CStringLoader::CStringLoader(
	__in_opt HMODULE hModule,	/* Handle to the module containing the resource */
	__in LPCTSTR resType,		/* Resource type */
	__in LPCTSTR resName		/* Resource name */)
{
	DWORD bufferSize = 0;
	const void *buffer = Utils::LoadResource(hModule, resType, resName, &bufferSize);
	if (buffer && bufferSize)
	{
		std::string utf8Buffer;
		if (bufferSize > 3 && ((BYTE*)buffer)[0] == 0xEF && ((BYTE*)buffer)[1] == 0xBB && ((BYTE*)buffer)[2] == 0xBF)
			// UTF8 encoded with BOM
			utf8Buffer.append((char*)buffer + 3, bufferSize - 3);
		else
			utf8Buffer.append((char*)buffer, bufferSize);

		InitStringsFromXMLContent(utf8Buffer.c_str());
	}
}

Utils::CStringLoader::CStringLoader(
	__in const char *xmlContent /* The UTF8-encoded XML content */)
{
	InitStringsFromXMLContent(xmlContent);
}

Utils::CStringLoader::~CStringLoader()
{

}

_tstring Utils::CStringLoader::LoadString(
	__in const char *stringID, /* The ID of the string to be loaded */
	__in_opt const char *langID /*= NULL /* /* The language ID, specify NULL for the default (unnamed) language */)
{
	_tstring result;

	return LoadString(stringID, result, langID) ? result : _T("");		
}

bool Utils::CStringLoader::LoadString(
	__in const char *stringID, /* The ID of the string to be loaded */
	__out _tstring &stringContent, /* The variable containing the string content on return */ 
	__in_opt const char *langID /*= NULL /* The language ID, specify NULL for the default (unnamed) language */)
{
	if (stringID == NULL) return false;
	CStringIdMap::iterator itor = m_StringIdMap.find(stringID);
	if (itor == m_StringIdMap.end()) return false;	// ID not found

	std::string languageId(langID ? langID : "");	// Use empty string for default language

	CStringLangMap::iterator stringItor = itor->second.find(languageId);
	if (stringItor == itor->second.end())
	{
		if (languageId.empty()) return false;

		// Try loading string from default language in case the specified language string could not be found
		stringItor = itor->second.find("");
		if (stringItor == itor->second.end()) return false;
	}


	stringContent = stringItor->second;	// 
	return true;

}

bool Utils::CStringLoader::InitStringsFromXMLContent(
	__in const char *xmlContent /* The UTF8-encoded XML content */)
{
	if (xmlContent == NULL) return false;

	m_StringIdMap.clear();
	Utils::CMsXmlDoc *xmlDoc = Utils::CMsXmlDoc::ParseFromUtf8String(xmlContent);
	if (xmlDoc == NULL)
		return false;

	Utils::CMsXmlNode *rootNode = xmlDoc->GetRoot();

	if (rootNode == NULL)
	{
		delete xmlDoc;
		return false;
	}

	bool result = false;
	_tstring name = rootNode->GetName();
	if (_tcsicmp(name.c_str(), _T("text_table")) == 0)
	{
		std::vector<Utils::CMsXmlNode*> children = rootNode->GetChildren();

		for (Utils::CMsXmlNode *child : children)
		{
			name = child->GetName();
			if (_tcsicmp(name.c_str(), _T("text")) == 0)
			{
				std::map<_tstring, _tstring> attribs = child->GetAttributes();
				std::map<_tstring, _tstring>::iterator itor = attribs.find(_T("id"));
				if (itor != attribs.end() && !itor->second.empty())
				{
					std::string stringID = Utils::MakeLowerA(TSTRING_TO_UTF8(itor->second.c_str()));
					if (!stringID.empty())
					{
						std::vector<Utils::CMsXmlNode*> textEntries = child->GetChildren();
						for (Utils::CMsXmlNode *entry : textEntries)
						{
							name = entry->GetName();
							if (_tcsicmp(name.c_str(), _T("entry")) == 0)
							{
								std::map<_tstring, _tstring> entryAttribs = entry->GetAttributes();
								std::string stringLang;
								for (auto entryAttr : entryAttribs)
								{
									if (_tcsicmp(entryAttr.first.c_str(), _T("lang")) == 0)
										stringLang = TSTRING_TO_UTF8(entryAttr.second.c_str());
									
								}
								if (!stringLang.empty())
									stringLang = Utils::MakeLowerA(stringLang);
								_tstring stringValue = entry->GetStringValue();
								AddStringItem(stringID.c_str(), stringValue.c_str(), stringLang.c_str());
							}
							delete entry;
						}						
					}
				}				
			}
			delete child;
		}
	}
	delete rootNode;

	delete xmlDoc;
	return result;
}

// Add a string item
void Utils::CStringLoader::AddStringItem(
	__in const char *stringID,
	__in LPCTSTR stringValue,
	__in_opt const char *langID /* = NULL*/)
{
	if (stringID == NULL || stringValue == NULL) return;

	std::string lang = langID ? langID : "";

	CStringIdMap::iterator itor = m_StringIdMap.find(stringID);
	if (itor == m_StringIdMap.end())
	{
		// New string ID
		CStringLangMap langMap;
		langMap[lang] = stringValue;

		m_StringIdMap[stringID] = langMap;
	}
	else
	{
		itor->second[lang] = stringValue;
	}
}
