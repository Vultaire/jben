#include "radkfile.h"

RadKFile *RadKFile::LoadRadK(const char* filename, int& result) {
	result = RADK_FILENOTFOUND;
	return NULL;
}

RadKFile::RadKFile(char* radkfileRawData) {
}
