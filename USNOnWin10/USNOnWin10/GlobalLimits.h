#ifndef GLOBAL_LIMITS_H
#define GLOBAL_LIMITS_H

#include <Windows.h>

constexpr DWORD BUFFER_SIZE = (1024 * 1024);

constexpr WCHAR VOLUME_ROOT_SYMBOL[7] = L"\\\\?\\f:";

constexpr DWORD VOLUME_ROOT_SYMBOL_SIZE = 7;

constexpr DWORD VOLUME_ROOT_SYMBOL_POS = 4;

#endif  // GLOBAL_LIMITS_H
