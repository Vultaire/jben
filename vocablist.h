/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: vocablist.h

This file is part of J-Ben.

J-Ben is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

J-Ben is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef vocablist_h
#define vocablist_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

/*#include "kanjidic.h"*/
#include <set>
using namespace std;

#define ST_GRADE     1
#define ST_FREQUENCY 2

class VocabList {
public:
	VocabList();
	void Add(const wxString& s);
	int AddList(const wxString& s);
	wxString ToString(wxChar separator=_T('\n'));
	int Size();
	void Clear();
	const wxString& operator[](unsigned int index);
	int GetIndexByWord(const wxString& s);
	set<wxString>& GetVocabList();
private:
	set<wxString> vocabList;
};

#endif
