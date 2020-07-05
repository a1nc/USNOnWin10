// USNOnWin10.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "GlobalLimits.h"
#include "InteractiveUtils.h"
#include "StringUtils.h"
#include "UsnUtils.h"

#include <WinIoCtl.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <set>
#include <string>

using namespace std;
HANDLE drive;
USN maxusn;

void FindUSNRecordRecursively(USN_RECORD *record, set<DWORDLONG> &memo, DWORDLONG& filecount) {
  if (record == nullptr) {
    return;
  }
  if (memo.count(record->FileReferenceNumber) != 0) {
    return;
  }
  memo.insert(record->FileReferenceNumber);
  std::cout << "memo size is : " << memo.size() << std::endl;
  PrintUSNRecord(record);
  filecount++;

  // construct the parnet file reference number MFT data
  MFT_ENUM_DATA_V0 mft{record->ParentFileReferenceNumber, 0, maxusn};

  void *buffer;
  DWORD bytecount = 1;

  buffer =
      VirtualAlloc(NULL, BUFFER_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (buffer == NULL) {
    printf("VirtualAlloc: %u\n", GetLastError());
    return;
  }

  if (!QueryMFTEnumData(drive, mft, buffer, BUFFER_SIZE, bytecount)) {
    printf("FSCTL_ENUM_USN_DATA (FindUSNRecordRecursively): %u\n",
           GetLastError());
    return;
  }

  USN_RECORD *parentRecord;
  parentRecord = (USN_RECORD *)((USN *)buffer + 1);
  if (parentRecord->FileReferenceNumber != record->ParentFileReferenceNumber) {
    return;
  } else {
    // WCHAR * filename;
    // WCHAR * filenameend;
    // filename = (WCHAR *)(((BYTE *)record) + record->FileNameOffset);
    // filenameend = (WCHAR *)(((BYTE *)record) + record->FileNameOffset +
    // record->FileNameLength); WCHAR * pfilename; WCHAR * pfilenameend;
    // pfilename = (WCHAR *)(((BYTE *)parentRecord) +
    // parentRecord->FileNameOffset); pfilenameend = (WCHAR *)(((BYTE
    // *)parentRecord) + parentRecord->FileNameOffset +
    // parentRecord->FileNameLength); printf("FileName: %.*ls\\%.*ls\n",
    // pfilenameend - pfilename, pfilename, filenameend - filename, filename);
    //
  }
  VirtualFree(buffer, 0, MEM_RELEASE);
  // FindUSNRecordRecursively(parentRecord, memo);
}

int main(int argc, char **argv) {
  std::ios::sync_with_stdio(false); 
  InitPrint(argc, argv);
  // Support run on windows cmd administartor's mode
  // USNOnWin10.exe [volumeSymbol]
  // eg:
  //      USNOnWin10.exe c
  // will scan the volume C: 's files
  if (argc == 2) {
    std::string params(argv[1]);
    if (params.size() == 1 && isalpha(params[0])) {
      WCHAR *wchPtr;
      DWORD wchSize;
      StringToWchart(params, wchPtr, wchSize);
      drive = CreateVolumeHandle(*(wchPtr));
    }
  }

  if (drive == INVALID_HANDLE_VALUE) {
    std::cout << "ERROR : Create volume handle error, " << GetLastError();
    return 0;
  }

  DWORD starttick;
  starttick = GetTickCount64();
  DWORD bytecount = 1;
  void *buffer =
      VirtualAlloc(NULL, BUFFER_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (buffer == NULL) {
    std::cout << "ERROR: Allocate memory error, " << GetLastError();
    return 0;
  }
  if (!QueryUSNJournal(drive, buffer, BUFFER_SIZE, bytecount)) {
    std::cout << "ERROR: QueryUSNJournal failed error, " << GetLastError()
              << std::endl;
    std::cout << "INFO: Try to create USN journl in this volume" << std::endl;
    if (drive == INVALID_HANDLE_VALUE) {
      std::cout << "ERROR : Create volume handle error, " << GetLastError();
      return 0;
    }
    if (!CreateUSNJouranl(drive)) {
      return 0;
    }
  }

  USN_JOURNAL_DATA *journal;
  journal = (USN_JOURNAL_DATA *)buffer;
  PrintUSNJournal(journal);

  maxusn = journal->MaxUsn;
  MFT_ENUM_DATA_V0 mft{0, 0, maxusn};
  USN_RECORD *record;
  USN_RECORD *recordend;
  DWORDLONG nextid = 0;
  DWORDLONG filecount = 0;

  for (;;) {
    set<DWORDLONG> memo;
    if (!QueryMFTEnumData(drive, mft, buffer, BUFFER_SIZE, bytecount)) {
      EndPrint(nextid, filecount, starttick);
      return 0;
    }
    nextid = *((DWORDLONG *)buffer);
    record = (USN_RECORD *)((USN *)buffer + 1);
    recordend = (USN_RECORD *)(((BYTE *)buffer) + bytecount);
    while (record < recordend) {
      FindUSNRecordRecursively(record, memo, filecount);
      record = (USN_RECORD *)(((BYTE *)record) + record->RecordLength);
    }
    //std::cout << "DEBUG ::::::: Next ID: " << nextid << std::endl;
    mft.StartFileReferenceNumber = nextid;
  }
  return 0;
}
