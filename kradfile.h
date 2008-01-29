#ifndef kradfile_h
#define kradfile_h

#define KRAD_SUCCESS      0x0
#define KRAD_FILENOTFOUND 0x1

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "boosthm.h"

class KRadFile {
public:
	static KRadFile *LoadKRad(const char* filename, int& result);
private:
	KRadFile(char *kradfileRawData);
	BoostHM<wxChar, wxString> kraddata;
};

#endif
