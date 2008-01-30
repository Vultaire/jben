/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: kanjilist.cpp

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

#include "kanjilist.h"
#include "kdict.h"
#include <algorithm>
using namespace std;

KanjiList::KanjiList(const BoostHM<wxChar,string>* const kDictHash) {
	kanjiHash = kDictHash;
}

#if 0
vector<wxChar>::iterator KanjiList::Find(wxChar c) {
	vector<wxChar>::iterator it;
	for(it=kanjiList.begin();it!=kanjiList.end();it++) {
		if(*it==c) break;
	}
	return it;
}
#endif

int KanjiList::AddFromString(const wxString& s) {
	int kanjiAdded = 0, len = s.length();
	wxChar c;
	BoostHM<wxChar,string>::const_iterator it;

	for(int i=0;i<len;i++) {
		c = s[i];
		it = kanjiHash->find(c);
		if(it!=kanjiHash->end()) {
			if(find(kanjiList.begin(), kanjiList.end(), c)==kanjiList.end()) {
				kanjiList.push_back(c);
				kanjiAdded++;
			}
		}
	}

	return kanjiAdded;
}

wxString KanjiList::ToString() {
	return ToString(0);
}

wxString KanjiList::ToString(int lineWidth) {
		wxString result;
		int lineWidthCounter=0;
		int len = kanjiList.size();
		for(int i=0;i<len;i++) {
			result.append(kanjiList[i]);
			if(lineWidth>0) {
				lineWidthCounter++;
				if(lineWidthCounter>=lineWidth) {
					result.append(_T('\n'));
					lineWidthCounter=0;
				}
			}
		}
		return result;
}

void KanjiList::Clear() {
	kanjiList.clear();
}

int KanjiList::AddByGrade(int lowGrade, int highGrade) {
	wxString kanjiStr;
	int grade;
	const KDict* kd = KDict::GetKDict();

	for(BoostHM<wxChar,string>::const_iterator ki=kanjiHash->begin(); ki!=kanjiHash->end(); ki++) {
		grade = kd->GetIntField(ki->first, _T("G"));
		if(grade>=lowGrade &&
		  (grade<=highGrade || highGrade==0))
			kanjiStr.append(ki->first);
	}

	return AddFromString(kanjiStr);
}

int KanjiList::AddByFrequency(int lowFreq, int highFreq) {
	wxString kanjiStr;
	int freq;
	const KDict* kd = KDict::GetKDict();

	for(BoostHM<wxChar,string>::const_iterator ki=kanjiHash->begin(); ki!=kanjiHash->end(); ki++) {
		freq = kd->GetIntField(ki->first, _T("F"));
		if(freq>=lowFreq && freq<=highFreq)
			kanjiStr.append(ki->first);
	}

	return AddFromString(kanjiStr);
}

int KanjiList::Size() {return kanjiList.size();}

void KanjiList::InplaceMerge(vector<wxChar>& v, BoostHM<wxChar,int>& indexer, int start, int middle, int end) {
	/* Merge is implemented as a bubble sort started at halfway
	   (since we know the first whole half is already sorted) */
	int i, highIndex;
	wxChar temp;
	i = highIndex = middle;
	while(i<end) {
		if(i>0 && (indexer[v[i]] < indexer[v[i-1]])) {
			temp = v[i-1];
			v[i-1] = v[i];
			v[i] = temp;
			i--;
		} else {
			highIndex++;
			i=highIndex;
		}
	}
}

/*
SortKanjiList sorts the currently loaded kanji list based upon a specified
KANJIDIC field, like F (frequency) or G (jouyou grade).  Sorting is done
via a merged sort.  This might be overkill, but I wanted to try doing
it, so I did.
*/
void KanjiList::Sort(int sortType, bool reverseOrder) {
	int totalSize = kanjiList.size();
	if(totalSize<=1) return;  /* Size 0 or 1 list is already sorted */

	myCharIndexer = new BoostHM<wxChar,int>;
	myCharIndexer->clear();
	vector<wxChar>::iterator vi;

	wxString fieldMarker;
	switch(sortType) {
	case ST_GRADE:
		fieldMarker=_T("G");
		break;
	case ST_FREQUENCY:
		fieldMarker=_T("F");
		break;
	default:
		fieldMarker=_T("INVALID");
	}

	/* Create index based on the sort type */
	int value;
	const KDict* kd = KDict::GetKDict();
	for(vi=kanjiList.begin();vi!=kanjiList.end();vi++) {
		value = kd->GetIntField(*vi, fieldMarker);
		if(value==-1) value=INT_MAX;
		myCharIndexer->assign(*vi, value);
	}

	/* Sort our data based upon the stored key in the hash table */
	/* This code, a merge sort, was created based upon code at:
	   http://en.wikipedia.org/wiki/Merge_sort#C.2B.2B_implementation
	   These pages were referred to:
	   http://www.cppreference.com/cppalgorithm/merge.html
	   http://www.cppreference.com/cppalgorithm/inplace_merge.html

	   The below is an original implementation designed to work with a wxHashMap and vector<wxChar>. */
	int rangeSize, rangeStart;
	for(rangeSize=1; rangeSize<totalSize; rangeSize *= 2) {
		for(rangeStart=0; rangeStart<totalSize-rangeSize; rangeStart += rangeSize*2) {
			/* Our range sort function is HERE */
			InplaceMerge(
			  kanjiList,
			  *myCharIndexer,
			  rangeStart,
			  rangeStart + rangeSize,
			  min(rangeStart + rangeSize*2, totalSize));
		}
	}

#if 0
	/* DEBUG ONLY: Check that the sort works as intended! */
	if(totalSize>0) {
		int lastVal = (*myCharIndexer)[kanjiList[0]];
		for(int i=1;i<totalSize;i++) {
			value = (*myCharIndexer)[kanjiList[i]];
			if(value<lastVal)
				fprintf(stderr, "Error! Index %d has value of %d, while %d has value of %d!\n", i, value, i-1, lastVal);
			else
				printf("%d=%d\t", i, value);
			lastVal = value;
		}
	}
#endif

	delete myCharIndexer;
}

#if 0
wxChar KanjiList::GetCharByIndex(unsigned int index) {
	return operator[](index);
}
#endif

wxChar KanjiList::operator[](unsigned int index) {
	if(index<kanjiList.size()) return kanjiList[index];
	return _T('\0');
}

int KanjiList::GetIndexByChar(wxChar c) {
	int i, len = kanjiList.size();
	for(i=0;i<len;i++)
		if(kanjiList[i]==c) return i;
	return -1;
}

vector<wxChar>& KanjiList::GetVector() {return kanjiList;}
