#ifndef radicals_h
#define radicals_h

#include "kradfile.h"
#include "radkfile.h"

class Radicals {
public:
	Radicals();
	int LoadRadK(const char* filename);
private:
	BoostHM<wxChar, wxString> radkdata;
};

#endif
