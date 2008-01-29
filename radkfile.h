#ifndef radkfile_h
#define radkfile_h

#define RADK_SUCCESS      0x0
#define RADK_FILENOTFOUND 0x1

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "boosthm.h"

class RadKFile {
public:
	static RadKFile *LoadRadK(const char* filename, int& result);
private:
	RadKFile(char *radkfileRawData);

	BoostHM<wxChar, wxString> radkdata;
};

#endif
