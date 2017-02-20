#include "stdafx.h"
#include "StringUtils.h"
#include <algorithm>

using namespace Utils;

BSTR Utils::SysAllocStringFromUtf8String(
	__in const char *pUtf8)
{
	if (pUtf8 == NULL) return NULL;

	int requiredSize = MultiByteToWideChar(CP_UTF8, 0, pUtf8, -1, NULL, 0);
	if (requiredSize == 0 || (unsigned int)requiredSize == 0xFFFD)
		return NULL;

	WCHAR *pUnicode = new WCHAR[requiredSize];
	if (pUnicode == NULL) return NULL;

	int converted = MultiByteToWideChar(CP_UTF8, 0, pUtf8, -1, pUnicode, requiredSize);
	if (converted == 0 || (unsigned int)converted == 0xFFFD)
	{
		delete[] pUnicode;
		return NULL;
	}

	BSTR result = SysAllocString(pUnicode);
	delete[] pUnicode;

	return result;
}

std::wstring Utils::Utf8ToUnicode(
	__in const char *pUtf8)
{
	if (pUtf8 == NULL) return L"";

	int requiredSize = MultiByteToWideChar(CP_UTF8, 0, pUtf8, -1, NULL, 0);
	if (requiredSize == 0 || (unsigned int)requiredSize == 0xFFFD)
		return NULL;

	WCHAR *pUnicode = new WCHAR[requiredSize];
	if (pUnicode == NULL) return NULL;

	int converted = MultiByteToWideChar(CP_UTF8, 0, pUtf8, -1, pUnicode, requiredSize);
	if (converted == 0 || (unsigned int)converted == 0xFFFD)
	{
		delete[] pUnicode;
		return NULL;
	}

	std::wstring result;
	result.append(pUnicode, converted - 1);
	delete[] pUnicode;
	return result;
}

std::wstring Utils::Utf8ToUnicode(
	__in const std::string &utf8)
{
	return Utf8ToUnicode(utf8.c_str());
}

std::string Utils::UnicodeToUtf8(
	__in const WCHAR *pUnicode)
{
	if (pUnicode == NULL) return "";

	int requiredSize = WideCharToMultiByte(CP_UTF8, 0, pUnicode, -1, NULL, 0, NULL, NULL);
	if (requiredSize == 0 || (unsigned int)requiredSize == 0xFFFD)
		return NULL;

	char *pUtf8 = new char[requiredSize];
	if (pUtf8 == NULL) return NULL;

	int converted = WideCharToMultiByte(CP_UTF8, 0, pUnicode, -1, pUtf8, requiredSize, NULL, NULL);
	if (converted == 0 || (unsigned int)converted == 0xFFFD)
	{
		delete[] pUtf8;
		return NULL;
	}


	std::string result;
	result.append(pUtf8, converted - 1);
	delete[] pUtf8;

	return result;
}

std::string Utils::UnicodeToUtf8(
	__in const std::wstring &unicode)
{
	return UnicodeToUtf8(unicode.c_str());
}


bool Utils::ParseBoolConfig(
	__in const char *configValue,
	__out bool &result)
{
	if (configValue == NULL) return false;

	if (_stricmp(configValue, "true") == 0 ||
		_stricmp(configValue, "yes") == 0 ||
		_stricmp(configValue, "1") == 0)
	{
		result = true;
		return true;
	}
	else if (_stricmp(configValue, "false") == 0 ||
		_stricmp(configValue, "no") == 0 ||
		_stricmp(configValue, "0") == 0)
	{
		result = false;
		return true;
	}

	return false;
}

bool Utils::ParseLongConfig(
	__in const char *configValue,
	__out long &result,
	int radix /* = 10*/)
{
	if (configValue == NULL) return false;

	char *pos = 0;
	long val = strtol(configValue, &pos, radix);
	if (pos == NULL || pos[0]) return false;

	result = val;
	return true;
}

bool Utils::ParseULongConfig(
	__in const char *configValue,
	__out unsigned long &result,
	int radix /*= 10*/)
{
	if (configValue == NULL) return false;

	char *pos = 0;
	long val = strtoul(configValue, &pos, radix);
	if (pos == NULL || pos[0]) return false;

	result = val;
	return true;
}

std::string Utils::MakeLowerA(__in const char *str)
{
	if (str) return MakeLowerA(std::string(str));
	return "";
}

std::string Utils::MakeLowerA(__in const std::string &str)
{
	std::string result = str;
	std::transform(str.begin(), str.end(), result.begin(), tolower);

	return result;
}

std::string Utils::MakeUpperA(__in const char *str)
{
	if (str) return MakeUpperA(std::string(str));
	return "";
}

std::string Utils::MakeUpperA(__in const std::string &str)
{
	std::string result;
	std::transform(str.begin(), str.end(), result.begin(), toupper);

	return result;
}

std::wstring Utils::MakeLowerW(__in const WCHAR *str)
{
	if (str) return MakeLowerW(std::wstring(str));
	return L"";
}

std::wstring Utils::MakeLowerW(__in const std::wstring &str)
{
	std::wstring result = str;
	std::transform(str.begin(), str.end(), result.begin(), towlower);

	return result;
}

std::wstring Utils::MakeUpperW(__in const WCHAR *str)
{
	if (str) return MakeUpperW(std::wstring(str));
	return L"";
}

std::wstring Utils::MakeUpperW(__in const std::wstring &str)
{
	std::wstring result;
	std::transform(str.begin(), str.end(), result.begin(), towupper);

	return result;
}
