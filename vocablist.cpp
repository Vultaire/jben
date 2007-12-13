/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: vocablist.cpp

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

#include "vocablist.h"
#include "global.h"
#include "wx/tokenzr.h"

VocabList::VocabList() {
/*	vocabList.reserve(1000); */ /* We might want to do this later...? */
}

/* "BinarySearch" searches for a query string and returns its index.
   If the string is not found, it returns the index at which the query
   should be inserted.  matchFound indicates whether the return value is
   a match or merely an insertion point. */
int VocabList::BinarySearch(const wxString& query, bool* matchFound) {
	int listLength = vocabList.size();
	int lowBound = 0, highBound = listLength-1;
	int compareVal=0, index=0;

	/* Special case: empty list */
	if(listLength<=0) {
		if(matchFound) *matchFound = false;
		return 0;
	}

	/* Search for a match.  Return when found. */
	while(lowBound <= highBound) {
		index = (lowBound+highBound)/2;
		compareVal = query.compare(vocabList[index]);
		if(compareVal==0) {
			if(matchFound) *matchFound = true;
			return index;
		} else if(compareVal<0) {
			highBound = index-1;
		} else {
			lowBound = index+1;
		}
	}

	/* Match was not found.  "index" will be our insertion point, and
	   matchFound is false. */
	if(matchFound) *matchFound = false;
	if(compareVal>0) index++; /* If query is greater than the final item we
								 looked at, then we want to insert
								 afterwards. */
	return index;
}

/* "Add" inserts an item into the study list.
   It uses a custom binary search function to find an insertion point for the
   string, and to check if the string is already in the list.
   No duplicate strings are allowed in the list, and such duplicates are quietly
   discarded. */
bool VocabList::Add(const wxString& s) {
	bool matchFound;
	int insertPoint;

	insertPoint = BinarySearch(s, &matchFound);
	if(!matchFound) {
		vocabList.insert(vocabList.begin() + insertPoint, s);
		return true;
	}
	return false;
}

int VocabList::AddList(const wxString& s) {
	wxStringTokenizer t(s, _T("\n;"));
	wxString token;
	int count = 0;
	int duplicates = 0;
	while(t.HasMoreTokens()) {
		token = t.GetNextToken();
		if(token.length()>0) {
			if(Add(token)) count++;
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

		vector<wxString>::iterator it = vocabList.begin();
		if(it!=vocabList.end()) {
			result.append(*it);
			it++;
			for(; it!=vocabList.end(); it++) {
				result.append(separator);
				result.append(*it);
			}
		}

		return result;
}

void VocabList::Clear() {
	vocabList.clear();
}

int VocabList::Size() {return vocabList.size();}

const wxString& VocabList::operator[](unsigned int index) {
	/* new, simpler code, with index being 0-based */
	return vocabList[index];
	/* old code follows */
#if 0
	/* "index" is 1-based. */
	vector<wxString>::iterator it = vocabList.begin();
	for(unsigned int i=1;i<=index;i++) {
		if(it!=vocabList.end()) it++;
	}
	return *it;
#endif
}

int VocabList::GetIndexByWord(const wxString& s) {
	/* Index returned should be 0-based. -1 indicates not found. */
	bool found;
	int index = BinarySearch(s, &found);
#ifdef DEBUG
	printf("GetIndexByWord: BinarySearch (%ls) returned found=%d and index=%d.\n", s.c_str(), (int)found, index);
#endif
	if(!found) return -1;
	return index;
}

vector<wxString>& VocabList::GetVocabList() {return vocabList;}
