#pragma once

#include <map>
#include "Utils\MsXmlUtils.h"

namespace Utils
{
	class CStringLoader
	{
	private:
		// Data
		
		typedef std::map <std::string, _tstring> CStringLangMap; /* Map between language IDs and string contents */
		typedef std::map <std::string, CStringLangMap> CStringIdMap; /* Map between string IDs and CStringLangMap objects*/

		CStringIdMap m_StringIdMap;
	public:
		// Load strings from XML resource
		CStringLoader(
			__in_opt HMODULE hModule, /* Handle to the module containing the resource */
			__in LPCTSTR resType, /* Resource type */
			__in LPCTSTR resName /* Resource name */
			);

		// Load strings directly from UTF8-encoded XML content 
		CStringLoader(
			__in const char *xmlContent	/* The UTF8-encoded XML content */
			);

		~CStringLoader();
	public:
		// Load the specified string (return an empty string in case of errors)
		_tstring GetString(			
			__in const char *stringID, /* The ID of the string to be loaded */
			__in_opt const char *langID	= NULL /* The language ID, specify NULL for the default (unnamed) language */
			);

		// Load the specified string
		// Return true if successful, false otherwise
		// The result will be stored in stringContent
		bool GetString(
			__in const char *stringID, /* The ID of the string to be loaded */
			__out _tstring &stringContent, /* The variable containing the string content on return */
			__in_opt const char *langID = NULL /* The language ID, specify NULL for the default (unnamed) language */
			);

	private:
		// Initialize string table from UTF8-encoded XML content
		bool InitStringsFromXMLContent(
			__in const char *xmlContent	/* The UTF8-encoded XML content */);

		// Add a string item
		void AddStringItem(
			__in const char *stringID,
			__in LPCTSTR stringValue,
			__in_opt const char *langID = NULL);
	};


}