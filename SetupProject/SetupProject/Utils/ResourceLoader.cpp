#include "stdafx.h"
#include "Utils/ResourceLoader.h"

const void * Utils::LoadResource(
	__in_opt HMODULE hModule,
	__in LPCTSTR resType,
	__in LPCTSTR resName,
	__out_opt LPDWORD pResSize)
{
	if (resType == NULL || resName == NULL) return NULL;

	// Load the specified resource from the given module
	HRSRC hRsrc = FindResource(hModule, resName, resType);
	if (hRsrc == NULL) return NULL;

	DWORD resSize = SizeofResource(hModule, hRsrc);
	if (resSize == 0) return NULL;

	HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);
	if (hGlobal == NULL) return NULL;

	LPVOID buffer = LockResource(hGlobal);
	if (buffer == NULL) return NULL;

	if (pResSize) *pResSize = resSize;	// Get resource size

	return buffer;
}

const char* Utils::LoadUtf8Resource(
	__in_opt HMODULE hModule,
	__in LPCTSTR resType,
	__in LPCTSTR resName,
	__out DWORD &stringLen)
{
	DWORD bufLen = 0;
	const BYTE *buf = (const BYTE*)LoadResource(hModule, resType, resName, &bufLen);
	if (buf == NULL || bufLen == 0)	
		return NULL;
	if (bufLen > 3 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF)
	{
		// UTF8 BOM
		stringLen = bufLen - 3;
		return (const char*)buf + 3;
	}

	stringLen = bufLen;
	return (const char*)buf;
}
