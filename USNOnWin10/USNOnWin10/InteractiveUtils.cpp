#include "InteractiveUtils.h"

void InitPrint(int argc, char** argv) {
	std::cout << "USN simple tools running." << std::endl;
	std::cout << "Input params : " << std::endl;
	for (int i = 0; i < argc; i++) {
		std::cout << i << " " << argv[i] << std::endl;
	}
	std::cout << "===========================" << std::endl;
}

void EndPrint(DWORD nextid, DWORD filecount, DWORD starttick) {
	printf("===========================end================================\n");
	printf("FSCTL_ENUM_USN_DATA: %u\n", GetLastError());
	printf("Final ID: %lu\n", nextid);
	printf("File count: %lu\n", filecount);
	DWORD endtick = GetTickCount();
	printf("Ticks: %u\n", endtick - starttick);
}

WCHAR GetTargetVolumeSymbolFromArgs(int argc, char** argv) {
	DWORD volumeNameLen = 7;
	DWORD volumeNameKeyPos = volumeNameLen - 3;
	if (argc < 2 || argv == nullptr) {
		return DEFAULT_VOLUME_STR[volumeNameKeyPos];
	}
	std::string volumeSymbol(argv[1]);
	if (volumeSymbol.size() == 1 && isalpha(volumeSymbol[0])) {
		return tolower(volumeSymbol[0]);
	}
	return DEFAULT_VOLUME_STR[volumeNameKeyPos];
}
