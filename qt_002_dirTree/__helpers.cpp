#include "__helpers.h"



void convert_from_MultiByte_to_WideChar(std::wstring &destStr, const char *src)
{
	setlocale(LC_ALL, "Russian");

	const size_t maxSize = _MAX_PATH + 1;
	size_t pReturnValue;
	wchar_t dest[maxSize];

	mbstowcs_s(&pReturnValue, dest, maxSize, src, strlen(src));
	destStr = dest;

	return;
}
