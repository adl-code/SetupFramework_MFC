#pragma once

namespace Utils
{
	const void *LoadResource(
		__in_opt HMODULE hModule,
		__in LPCTSTR resType,
		__in LPCTSTR resName,
		__out_opt LPDWORD pResSize);

	const char* LoadUtf8Resource(
		__in_opt HMODULE hModule,
		__in LPCTSTR resType,
		__in LPCTSTR resName,
		__out DWORD &stringLen);
}