// USNOnWin10.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <WinIoCtl.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include "StringUtils.h"
#include "UsnUtils.h"
#include "InteractiveUtils.h"

#define BUFFER_SIZE (1024 * 1024)
using namespace std;
HANDLE drive;
USN maxusn;

void FindUSNRecordRecursively(USN_RECORD * record, set<DWORDLONG>& memo)
{
	if (record == nullptr) {
		return;
	}
	if (memo.count(record->FileReferenceNumber) != 0) {
		return;
	}
	memo.insert(record->FileReferenceNumber);
	PrintUSNRecord(record);
	// construct the parnet file reference number MFT data
	MFT_ENUM_DATA_V0 mft{ record->ParentFileReferenceNumber, 0, maxusn };
	
	void * buffer;
	DWORD bytecount = 1;

	buffer = VirtualAlloc(NULL, BUFFER_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (buffer == NULL) {
		printf("VirtualAlloc: %u\n", GetLastError());
		return;
	}

	if (!QueryMFTEnumData(drive, mft, buffer, BUFFER_SIZE, bytecount)){
		printf("FSCTL_ENUM_USN_DATA (FindUSNRecordRecursively): %u\n", GetLastError());
		return;
	}

	USN_RECORD * parentRecord;
	parentRecord = (USN_RECORD *)((USN *)buffer + 1);
	if (parentRecord->FileReferenceNumber != record->ParentFileReferenceNumber) {
		return;
	}
	FindUSNRecordRecursively(parentRecord, memo);
}

int main(int argc, char ** argv) {
	InitPrint(argc, argv);
	DWORD bytecount = 1;
	void * buffer;
	DWORD starttick;
	starttick = GetTickCount();

	buffer = VirtualAlloc(NULL, BUFFER_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (buffer == NULL) {
		std::cout << "ERROR: Allocate memory error, " << GetLastError();
		return 0;
	}

	WCHAR volumeName[7] = L"\\\\?\\f:";
	WCHAR volumeSymbol = GetTargetVolumeSymbolFromArgs(argc, argv);
	volumeName[7 - 3] = volumeSymbol;
	std::string outstr;
	WchartToString(volumeName, outstr);
	std::cout << "Init target volume is " << outstr << std::endl;

	drive = CreateVolumeHandle(volumeName);
	if (drive == INVALID_HANDLE_VALUE) {
		std::cout << "ERROR : Create volume handle error : " << GetLastError();
		return 0;
	}

	if (!QueryUSNJournal(drive, buffer, BUFFER_SIZE, bytecount)) {
		printf("FSCTL_QUERY_USN_JOURNAL: %u\n", GetLastError());
		return 0;
	}

	USN_JOURNAL_DATA * journal;
	journal = (USN_JOURNAL_DATA *)buffer;
	PrintUSNJournal(journal, 1);

	maxusn = journal->MaxUsn;
	MFT_ENUM_DATA_V0 mft{0, 0, maxusn};
	set<DWORDLONG> memo;

	USN_RECORD * record;
	USN_RECORD * recordend;
	DWORDLONG nextid;
	DWORDLONG filecount = 0;
	for (;;) {
		if (!QueryMFTEnumData(drive, mft, buffer, BUFFER_SIZE, bytecount)) {
			EndPrint(nextid, filecount, starttick);
			return 0;
		}
		nextid = *((DWORDLONG *)buffer);
		printf("Next ID: %lu\n", nextid);
		record = (USN_RECORD *)((USN *)buffer + 1);
		recordend = (USN_RECORD *)(((BYTE *)buffer) + bytecount);
		while (record < recordend) {
			filecount++;
			FindUSNRecordRecursively(record, memo);
			record = (USN_RECORD *)(((BYTE *)record) + record->RecordLength);
		}
		mft.StartFileReferenceNumber = nextid;
	}
	return 0;
}
