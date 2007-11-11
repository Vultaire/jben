/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: vocablist.cpp

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

#include "vocablist.h"
#include "global.h"
#include "wx/tokenzr.h"

VocabList::VocabList() {
}

void VocabList::Add(const wxString& s) {
	if(vocabList.insert(s).second==false) {
		wxMessageBox(wxString(s).append(_T(" was not inserted, since it is already in the list.")), _T("Notice"), wxOK | wxICON_INFORMATION, jben->gui);
	}
}

int VocabList::AddList(const wxString& s) {
	wxStringTokenizer t(s, _T("\n;"));
	wxString token;
	int count = 0;
	int duplicates = 0;
	while(t.HasMoreTokens()) {
		token = t.GetNextToken();
		if(token.length()>0) {
			if(vocabList.insert(token).second) count++;
			else duplicates++;
		}
	}
	if(duplicates>0)
		wxMessageBox(
		wxString::Format(
			_T("%d duplicate entries were detected, and were therefore ommitted."),
			duplicates),
		_T("Notice"), wxOK | wxICON_INFORMATION, jben->gui);
	return count;
}

wxString VocabList::ToString(wxChar separator) {
		wxString result;
		int i=0;
		set<wxString>::iterator it=vocabList.begin();
		if(it!=vocabList.end()) {
			result.append(*it);
			i++;
			for(++it; it!=vocabList.end(); it++) {
				result.append(separator);
				result.append(*it);
				i++;
			}
		}
		return result;
}

void VocabList::Clear() {
	vocabList.clear();
}

int VocabList::Size() {return vocabList.size();}

const wxString& VocabList::operator[](unsigned int index) {
	set<wxString>::iterator it = vocabList.begin();
	for(unsigned int i=1;i<=index;i++) {
		if(it!=vocabList.end()) it++;
	}
	return *it;
}

int VocabList::GetIndexByWord(const wxString& s) {
	/*set<wxString>::iterator it = vocabList.find(s);*/

	/* Yes, this is a horrible solution.  Probably it'll be faster to
	   redefine vocabList as a vector, and start by creating the vocabList
	   as a set and then copying it over, just as is done with the Edict
	   constructor.  This code is just to get things working. */

#ifdef DEBUG
	printf("DEBUG: Searching for \"%ls\"...\n", s.c_str());
#endif
	set<wxString>::iterator it = vocabList.begin();
	if(it==vocabList.end()) return -1;
	if(*it==s) {
#ifdef DEBUG
		printf("Search of \"%ls\" found \"%ls\" at index 0.\n", s.c_str(), it->c_str());
#endif
		return 0;
	}

	int i=1;
	for(it++; it!=vocabList.end(); it++) {
		if(*it==s) {
#ifdef DEBUG
			printf("Search of \"%ls\" found \"%ls\" at index %d.\n", s.c_str(), it->c_str(), i);
#endif
			return i;
		}
		i++;
	}

	return -1;
}

set<wxString>& VocabList::GetVocabList() {return vocabList;}
