/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: kanjilist.h

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

#ifndef kanjilist_h
#define kanjilist_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "kanjidic.h"
#include "boosthm.h"
#include <vector>
using namespace std;

#define ST_GRADE     1
#define ST_FREQUENCY 2

class KanjiList {
public:
	KanjiList(const BoostHM<wxChar,string> *kanjiDicHash);
	int AddFromString(const wxString& s);
	int AddByGrade(int lowGrade, int highGrade);
	int AddByFrequency(int lowFreq, int highFreq);
	wxString ToString();
	wxString ToString(int lineWidth);
	int Size();
#if 0
	vector<wxChar>::iterator Find(wxChar c);
#endif
	void Clear();
	void Sort(int sortType, bool reverseOrder=false);
	wxChar operator[](unsigned int index);
#if 0
	wxChar GetCharByIndex(unsigned int index);
#endif
	int GetIndexByChar(wxChar c);
	vector<wxChar>& GetVector();

private:
	void InplaceMerge(vector<wxChar>& v, BoostHM<wxChar,int>& indexer, int start, int middle, int end);

	const BoostHM<wxChar,string> *kanjiHash;
	vector<wxChar> kanjiList;
	BoostHM<wxChar, int> *myCharIndexer;
	/*  NOTE:
		The wxWidgets documentation does say to avoid using templates, but at
		the same time, wx has been around for a -long- time, and since gcc is
		pretty well universally supported and supports templates, I think for
		the purposes of my program the convenience of templates outweighs the
		compatibility loss with certain seldom-used obscure compilers.

		If desired, we can do the hunky-dory wxWidgets ARRAY mechanism,
		or we can just create a custom vector class by hand.  But until the STL
		class fails us, I see no reason not to use it. */
};

#endif
