#ifndef radkfile_h
#define radkfile_h

#define RADK_SUCCESS      0x0
#define RADK_FILENOTFOUND 0x1

class RadKFile {
public:
	int LoadRadK(const char* filename);
private:
	RadKFile(char *radkfileRawData);
	BoostHM<wxChar, wxString> radkdata;
};

#endif
