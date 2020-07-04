#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <Windows.h>
#include <string>

void WchartToString(WCHAR* wcharPtr, std::string& resStr);

void StringToWchart(std::string& inStr, WCHAR*& wcharPtr, DWORD& strSize);

#endif //STRING_UTILS_H
