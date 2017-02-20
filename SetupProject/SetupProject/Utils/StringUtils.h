#pragma once

#include <string>

// Define a general macro for std::string and std::wstring
#if !defined _tstring
#if defined _UNICODE || defined UNICODE
#define _tstring std::wstring
#else // Not unicode
#define _tstring std::string
#endif

#endif // define _tstring

#if !defined UTF8_TO_TSTRING
#if defined _UNICODE || defined UNICODE
#define UTF8_TO_TSTRING(x)	Utils::Utf8ToUnicode(x)
#else // Not unicode
#define UTF8_TO_TSTRING(x)	(x)
#endif
#endif // defined UTF8_TO_TSTRING

#if !defined UNICODE_TO_TSTRING
#if defined _UNICODE || defined UNICODE
#define UNICODE_TO_TSTRING(x)	(x)
#else // Not unicode
#define UNICODE_TO_TSTRING(x)	Utils::UnicodeToUtf8(x)
#endif
#endif // defined UTF8_TO_TSTRING

#if !defined TSTRING_TO_UTF8
#if defined UNICODE || defined _UNICODE
#define TSTRING_TO_UTF8(x)	Utils::UnicodeToUtf8(x)
#else // Not unicode
#define TSTRING_TO_UTF8(x)	(x)
#endif
#endif // define TSTRING_TO_UTF8

#if !defined TSTRING_TO_UNICODE
#if defined UNICODE || defined _UNICODE
#define TSTRING_TO_UNICODE(x)	(x)
#else // Not unicode
#define TSTRING_TO_UNICODE(x)	Utils::Utf8ToUnicode(x)
#endif
#endif // define TSTRING_TO_UNICODE

#if !defined MakeLower
#if defined UNICODE || defined _UNICODE
#define MakeLower MakeLowerW
#else
#define MakeLower MakeLowerA
#endif
#endif // define MakeLower

#if !defined MakeUpper
#if defined UNICODE || defined _UNICODE
#define MakeUpper MakeUpperW
#else
#define MakeUpper MakeUpperA
#endif
#endif // define MakeUpper

namespace Utils
{
	BSTR SysAllocStringFromUtf8String(
		__in const char *pUtf8);

	std::wstring Utf8ToUnicode(
		__in const char *pUtf8);

	std::wstring Utf8ToUnicode(
		__in const std::string &utf8);

	std::string UnicodeToUtf8(
		__in const WCHAR *pUnicode);

	std::string UnicodeToUtf8(
		__in const std::wstring &unicode);

	bool ParseBoolConfig(
		__in const char *configValue,
		__out bool &result);

	bool ParseLongConfig(
		__in const char *configValue,
		__out long &result,
		int radix = 10);
	bool ParseULongConfig(
		__in const char *configValue,
		__out unsigned long &result,
		int radix = 10);

	std::string MakeLowerA(
		__in const char *str);

	std::string MakeLowerA(
		__in const std::string &str);

	std::string MakeUpperA(
		__in const char *str);

	std::string MakeUpperA(
		__in const std::string &str);

	std::wstring MakeLowerW(
		__in const WCHAR *str);

	std::wstring MakeLowerW(
		__in const std::wstring &str);

	std::wstring MakeUpperW(
		__in const WCHAR *str);

	std::wstring MakeUpperW(
		__in const std::wstring &str);
}
