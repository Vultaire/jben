#include "kradfile.h"

KRadFile *KRadFile::LoadKRad(const char* filename, int& result) {
	result = KRAD_FILENOTFOUND;
	return NULL;
}

KRadFile::KRadFile(char *kradfileRawData) {
}
