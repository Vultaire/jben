#ifndef dictionaries_h
#define dictionaries_h

#include "wdict.h"
#include "kdict.h"
#include "kradfile.h"
#include "radkfile.h"

class Dictionaries {
public:
	Dictionaries();
	~Dictionaries();
	bool LoadWDict(const char* filename);
	bool LoadKDict(const char* filename);
	bool LoadKRadFile(const char* filename);
	bool LoadRadKFile(const char* filename);
	/* constant access to private vars */
	const WDict* GetWDict();
	const KDict* GetKDict();
	const KRadFile* GetKRadFile();
	const RadKFile* GetRadKFile();
private:
	WDict* wdict;
	const KDict* kdict;
	KRadFile* kradfile;
	RadKFile* radkfile;
};

#endif
