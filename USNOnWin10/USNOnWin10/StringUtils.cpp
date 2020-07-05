#include "StringUtils.h"

void WchartToString(WCHAR* wcharPtr, std::string& resStr) {
  if (wcharPtr == nullptr) {
    resStr = "";
    return;
  }
  WCHAR* tempWcharPtr = wcharPtr;
  DWORD byteNum = WideCharToMultiByte(CP_OEMCP, NULL, tempWcharPtr,
                                      -1, NULL, 0, NULL, FALSE);
  char* tempCharPtr = new char[byteNum];
  WideCharToMultiByte(CP_OEMCP, NULL, tempWcharPtr, -1,
                      tempCharPtr, byteNum, NULL, FALSE);
  resStr = tempCharPtr;
  delete[] tempCharPtr;
}

void StringToWchart(std::string& inStr, WCHAR*& wcharPtr, DWORD& strSize) {
  if (inStr.empty()) {
    strSize = 0;
    return;
  }
  std::string tempInStr = inStr;
  int wcharNum =
      MultiByteToWideChar(CP_ACP, 0, (LPCSTR)tempInStr.c_str(), -1, NULL, 0);
  WCHAR* tempWcharPtr = new WCHAR[wcharNum + 1];
  memset(tempWcharPtr, 0, wcharNum + 1);
  MultiByteToWideChar(CP_ACP, 0, (LPCSTR)tempInStr.c_str(), -1,
                      (LPWSTR)tempWcharPtr, wcharNum);
  wcharPtr = tempWcharPtr;
  strSize = wcharNum;
}
