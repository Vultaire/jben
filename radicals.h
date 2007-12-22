#ifndef radicals_h
#define radicals_h

#define KRAD_SUCCESS      0x0
#define KRAD_FILENOTFOUND 0x1
#define RADK_SUCCESS      0x0
#define RADK_FILENOTFOUND 0x1

class Radicals {
public:
	Radicals();
	int LoadKRad(const char* filename);
	int LoadRadK(const char* filename);
private:
	BoostHM<wxChar, wxString> radkdata;
	BoostHM<wxChar, wxString> kraddata;
};

#endif
