#include "dictionaries.h"
#include "preferences.h"

Dictionaries::Dictionaries() {
	wdict = NULL;
	kdict = NULL;
	kradfile = NULL;
	radkfile = NULL;

	/* Load file names from preferences object - TO DO */
	/* For now, just do a hard-coded load */

	LoadWDict("edict2");
	LoadKDict("kanjidic");
	LoadKRadFile("kradfile");
	LoadRadKFile("radkfile");
}

Dictionaries::~Dictionaries() {
	if(wdict) delete wdict;
	if(kdict) delete kdict;
	if(kradfile) delete kradfile;
	if(radkfile) delete radkfile;
}

bool Dictionaries::LoadWDict(const char* filename) {
	int result;
	wdict = WDict::LoadWDict(filename, result);
	if(result == ED_SUCCESS) return true;
	return false;
}

bool Dictionaries::LoadKDict(const char* filename) {
	int result;
	kdict = KDict::LoadKDict(filename, result);
	if(result == KD_SUCCESS) return true;
	return false;
}

bool Dictionaries::LoadKRadFile(const char* filename) {
	int result = 0xDEADBEEF;
	kradfile = KRadFile::LoadKRad(filename, result);
	if(result == KRAD_SUCCESS) return true;
	return false;
}

bool Dictionaries::LoadRadKFile(const char* filename) {
	int result = 0xDEADBEEF;
	radkfile = RadKFile::LoadRadK(filename, result);
	if(result == RADK_SUCCESS) return true;
	return false;
}

const WDict* Dictionaries::GetWDict() {return wdict;}
const KDict* Dictionaries::GetKDict() {return kdict;}
const KRadFile* Dictionaries::GetKRadFile() {return kradfile;}
const RadKFile* Dictionaries::GetRadKFile() {return radkfile;}
