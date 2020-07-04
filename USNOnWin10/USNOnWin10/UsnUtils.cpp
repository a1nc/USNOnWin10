#include "UsnUtils.h"

HANDLE CreateVolumeHandle(WCHAR volume[7]) {
	return CreateFile(volume,
						GENERIC_READ,
						FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_ALWAYS,
						FILE_FLAG_NO_BUFFERING,
						NULL);
}

bool QueryUSNJournal(HANDLE& volume, void*& buffer, DWORD bufferSize, DWORD& bytecount) {
	bool retStatus = DeviceIoControl(volume, 
						FSCTL_QUERY_USN_JOURNAL,
						NULL,
						0,
						buffer,
						bufferSize,
						&bytecount,
						NULL);
	return retStatus;
}

bool QueryMFTEnumData(HANDLE& volume, MFT_ENUM_DATA_V0& mft,
						void*& buffer, DWORD bufferSize, DWORD& bytecount) {
	bool retStatus = DeviceIoControl(volume,
						FSCTL_ENUM_USN_DATA,
						&mft,
						sizeof(mft),
						buffer,
						bufferSize,
						&bytecount,
						NULL);
	return retStatus;
}

//void FindUSNRecordRecursively(USN_RECORD * record, std::set<DWORDLONG>& memo) {
//	if (record == nullptr) {
//		return;
//	}
//	if (memo.count(record->FileReferenceNumber) != 0) {
//		return;
//	}
//	memo.insert(record->FileReferenceNumber);
//	PrintUSNRecord(record);
//	// construct the parnet file reference number MFT data
//	MFT_ENUM_DATA_V0 mft{ record->ParentFileReferenceNumber, 0, maxusn };
//
//	void * buffer;
//	DWORD bytecount = 1;
//
//	buffer = VirtualAlloc(NULL, BUFFER_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//	if (buffer == NULL) {
//		printf("VirtualAlloc: %u\n", GetLastError());
//		return;
//	}
//
//	if (!QueryMFTEnumData(drive, mft, buffer, BUFFER_SIZE, bytecount)) {
//		printf("FSCTL_ENUM_USN_DATA (FindUSNRecordRecursively): %u\n", GetLastError());
//		return;
//	}
//
//	USN_RECORD * parentRecord;
//	parentRecord = (USN_RECORD *)((USN *)buffer + 1);
//	if (parentRecord->FileReferenceNumber != record->ParentFileReferenceNumber) {
//		return;
//	}
//	FindUSNRecordRecursively(parentRecord, memo);
//}

void PrintUSNJournal(USN_JOURNAL_DATA* journal, DWORD detailLevel) {
	if (journal == nullptr) {
		printf("UsnJournalID: null\n");
		return;
	}
	printf("UsnJournalID: %lu\n", journal->UsnJournalID);
	printf("FirstUsn: %lu\n", journal->FirstUsn);
	printf("NextUsn: %lu\n", journal->NextUsn);
	printf("LowestValidUsn: %lu\n", journal->LowestValidUsn);
	printf("MaxUsn: %lu\n", journal->MaxUsn);
	printf("MaximumSize: %lu\n", journal->MaximumSize);
	printf("AllocationDelta: %lu\n", journal->AllocationDelta);
	if (detailLevel > 0) {
		printf("MinMajorVersion: %lu\n", journal->MinSupportedMajorVersion);
		printf("MaxMajorVersion: %lu\n", journal->MaxSupportedMajorVersion);
	}
}

void PrintUSNJournal(USN_JOURNAL_DATA& journal, DWORD detailLevel) {
	PrintUSNJournal(&journal, detailLevel);
}

void PrintMFT(MFT_ENUM_DATA_V0* mft, DWORD detailLevel) {
	if (mft == nullptr) {
		printf("MFT : null\n");
		return;
	}
	printf("StartFileReferenceNumber: %lu\n", mft->StartFileReferenceNumber);
	printf("LowUsn: %lu\n", mft->LowUsn);
	printf("HighUsn: %lu\n", mft->HighUsn);
}

void PrintUSNRecord(USN_RECORD* record, DWORD detailLevel) {
	if (record == nullptr) {
		printf("RecordLength: null\n");
		return;
	}
	printf("=================================================================\n");
	printf("RecordLength: %u\n", record->RecordLength);

	if (detailLevel > 0) {
		printf("MajorVersion: %u\n", (DWORD)record->MajorVersion);
		printf("MinorVersion: %u\n", (DWORD)record->MinorVersion);
	}
	printf("FileReferenceNumber: %lu\n", record->FileReferenceNumber);
	printf("ParentFRN: %lu\n", record->ParentFileReferenceNumber);
	printf("USN: %lu\n", record->Usn);

	if (detailLevel > 0) {
		printf("Timestamp: %lu\n", record->TimeStamp);
		printf("Reason: %u\n", record->Reason);
		printf("SourceInfo: %u\n", record->SourceInfo);
		printf("SecurityId: %u\n", record->SecurityId);
		printf("FileAttributes: %x\n", record->FileAttributes);
	}
	printf("FileNameLength: %u\n", (DWORD)record->FileNameLength);
	WCHAR * filename;
	WCHAR * filenameend;
	filename = (WCHAR *)(((BYTE *)record) + record->FileNameOffset);
	filenameend = (WCHAR *)(((BYTE *)record) + record->FileNameOffset + record->FileNameLength);
	printf("FileName: %.*ls\n", filenameend - filename, filename);
}

void PrintUSNRecord(USN_RECORD& record, DWORD detailLevel) {
	PrintUSNRecord(&record, detailLevel);
}

void PrintMFT(MFT_ENUM_DATA_V0& mft, DWORD detailLevel) {
	PrintMFT(&mft, detailLevel);
}

bool CheckUsnRecord(USN_RECORD * record) {
	WCHAR * filename;
	WCHAR * filenameend;

	filename = (WCHAR *)(((BYTE *)record) + record->FileNameOffset);
	filenameend = (WCHAR *)(((BYTE *)record) + record->FileNameOffset + record->FileNameLength);

	if (filenameend - filename != 8) {
		return false;
	}

	if (wcsncmp(filename, L"test.txt", 8) != 0) {
		return false;
	}
}
