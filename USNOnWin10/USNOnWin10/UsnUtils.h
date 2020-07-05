#ifndef USN_UTILS_H
#define USN_UTILS_H

#include <Windows.h>
#include <iostream>
#include <set>
#include <string>

HANDLE CreateVolumeHandle(WCHAR volume[7]);

HANDLE CreateVolumeHandle(WCHAR volumeWchar);

// void MakeVolumeSymbolByWchar(WCHAR symbol, WCHAR*& buffer, DWORD bufferSize);

bool GetNTFSVolumeInfo(HANDLE& volume, void*& buffer, DWORD bufferSize,
                       DWORD& retBytes);

bool CreateUSNJouranl(HANDLE& volume);

bool DeleteUSNJouranl(HANDLE& volume);

NTFS_EXTENDED_VOLUME_DATA GetNTFSVolumeInfo(HANDLE& volume);

bool QueryUSNJournal(HANDLE& volume, void*& buffer, DWORD buffer_size,
                     DWORD& bytecount);

bool QueryMFTEnumData(HANDLE& volume, MFT_ENUM_DATA_V0& mft, void*& buffer,
                      DWORD bufferSize, DWORD& bytecount);

// void FindUSNRecordRecursively(USN_RECORD * record, std::set<DWORDLONG>&
// memo);

void PrintUSNJournal(USN_JOURNAL_DATA* jounal, DWORD detailLevel = 0);

void PrintUSNJournal(USN_JOURNAL_DATA& jounal, DWORD detailLevel = 0);

void PrintMFT(MFT_ENUM_DATA_V0* mft, DWORD detailLevel = 0);

void PrintMFT(MFT_ENUM_DATA_V0& mft, DWORD detailLevel = 0);

void PrintUSNRecord(USN_RECORD* record, DWORD detailLevel = 0);

void PrintUSNRecord(USN_RECORD& record, DWORD detailLevel = 0);

bool CheckUsnRecord(USN_RECORD* record);

#endif  // USN_UTILS_H
