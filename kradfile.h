#ifndef kradfile_h
#define kradfile_h

#define KRAD_SUCCESS      0x0
#define KRAD_FILENOTFOUND 0x1

class KRadFile {
public:
	int LoadKRad(const char* filename);
private:
	KRadFile(char *kradfileRawData);
	BoostHM<wxChar, wxString> kraddata;
};

#endif
