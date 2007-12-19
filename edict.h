/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: edict.h

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef edict_h
#define edict_h

/* #define USING_INDICES */

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "boosthm.h"
#include <list>
#include <vector>
#include <utility>
using namespace std;

#define ED_SUCCESS 0x0
#define ED_FAILURE 0x80000000

#define EDS_EXACT 0x1u
#define EDS_BEGIN 0x2u
#define EDS_END   0x4u
#define EDS_ANY   0x8u

class Edict {
public:
	~Edict();
	static Edict *LoadEdict(const char *filename, int& returnCode);
	bool Search(const wxString& query, list<int>& results, unsigned int searchType
		= EDS_EXACT | (EDS_BEGIN << 8) | (EDS_END << 16) | (EDS_ANY << 24));
	wxString ResultToHTML(const wxString& rawResult);
	string GetEdictString(int i);
private:
	Edict(char *edictRawData);
	static wxString StripParenFields(const wxString& src);
	void GetEnglish(const string& edictStr, vector<string>& dest);
	void GetJapanese(const string& edictStr, vector<string>& dest);
	/*bool GetJapaneseReading(wxString& edictStr, vector<wxString>& dest);*/

	vector<string> edictData;

};

#endif
