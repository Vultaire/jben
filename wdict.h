/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: wdict.h

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

#ifndef wdict_h
#define wdict_h

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

class WDict {
public:
	static const WDict *GetWDict();
	static void Destroy();
	~WDict();

	/* General public EDICT functions
	   NOTE: Investigate why this was set as public.  Public functions should
	   not be tied to a specific dictionary file type. */
	string GetEdictString(int i) const;

	/* Dictionary search functions */
	bool Search(const wstring& query,
				list<int>& results,
				unsigned int searchType =
				EDS_EXACT | (EDS_BEGIN << 8) |
				(EDS_END << 16) | (EDS_ANY << 24)) const;
	static wstring ResultToHTML(const wstring& rawResult);

	/* Other functions */
	bool MainDataLoaded() const;
private:
	/* Hidden constructor */
	WDict();

	/* Dictionary file loaders */
	int LoadEdict2(const char *filename="edict2");

	/* EDICT2-specific stuff */
	void Edict2Parser(char *edict2RawData);

	/* General EDICT-compatible functions */
	static void GetEnglish(const string& edictStr, vector<string>& dest);
	static void GetJapanese(const string& edictStr, vector<string>& dest);

	/* Data */
	static WDict *wdictSingleton;
	vector<string> edictData;
};

#endif
