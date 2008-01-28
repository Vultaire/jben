#ifndef dictionaries_h
#define dictionaries_h

#include "edict.h"
#include "kanjidic.h"
#include "kradfile.h"
#include "radkfile.h"

class Dictionaries {
public:
	Dictionaries();
	~Dictionaries();
	bool LoadEdict   (const char* filename);
	bool LoadKanjidic(const char* filename);
	bool LoadKRadFile(const char* filename);
	bool LoadRadKFile(const char* filename);
	/* constant access to private vars */
	const Edict*    GetEdict();
	const Kanjidic* GetKanjidic();
	const KRadFile* GetKRadFile();
	const RadKFile* GetRadKFile();
private:
	Edict*    edict;
	Kanjidic* kanjidic;
	KRadFile* kradfile;
	RadKFile* radkfile;
};

#endif
