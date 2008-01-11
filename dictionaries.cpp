#include "dictionaries.h"
#include "preferences.h"

Dictionaries::Dictionaries() {
	wxString edictFile, kanjidicFile, kradFile, radkFile;
	
	edict    = NULL;
	kanjidic = NULL;
	kradfile = NULL;
	radkfile = NULL;

	/* Load file names from preferences object - TO DO */

	LoadEdict();
	LoadKanjiDic();
	LoadKRadFile();
	LoadRadKFile();
}

Dictionaries::~Dictionaries() {
	if(edict)    delete edict;
	if(kanjidic) delete kanjidic;
	if(kradfile) delete kradfile;
	if(radkfile) delete radkfile;
}

bool Dictionaries::LoadEdict(const char* filename) {
	int result;
	edict = KanjiDic::LoadEdict(filename, result);
	if(result == ED_SUCCESS) return true;
	return false;
}

bool Dictionaries::LoadKanjiDic(const char* filename) {
	int result;
	kanjidic = KanjiDic::LoadKanjidic(filename, result);
	if(result == KD_SUCCESS) return true;
	return false;
}

bool Dictionaries::LoadKRadFile(const char* filename) {
	kradfile = KRadFile::LoadKRadFile(filename, result);
	if(result == KRF_SUCCESS) return true;
	return false;
}

bool Dictionaries::LoadRadKFile(const char* filename) {
	radkfile = RadKFile::LoadRadKFile(filename, result);
	if(result == RKF_SUCCESS) return true;
	return false;
}

const Edict*    Dictionaries::GetEdict()    {return edict;}
const KanjiDic* Dictionaries::GetKanjiDic() {return kanjidic;}
const KRadFile* Dictionaries::GetKRadFile() {return kradfile;}
const RadKFile* Dictionaries::GetRadKFile() {return radkfile;}
