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
#include "errorlog.h"
#include <sstream>
#include <algorithm>
using namespace std;

KanjiList::KanjiList() {}

int KanjiList::AddFromString(const wstring& s) {
	int kanjiAdded = 0, len = s.length();
	wchar_t c;
	BoostHM<wchar_t,KInfo>::const_iterator it;

	const BoostHM<wchar_t, KInfo>* ht = KDict::Get()->GetHashTable();

	for(int i=0;i<len;i++) {
		c = s[i];
		it = ht->find(c);
		if(it!=ht->end()) {
			if(find(kanjiList.begin(), kanjiList.end(), c)==kanjiList.end()) {
				kanjiList.push_back(c);
				kanjiAdded++;
			}
		}
	}

	return kanjiAdded;
}

/* Convert the kanjilist into a wide char string,
   with lineWidth kanji per line (0 == no line breaks). */
wstring KanjiList::ToString(int lineWidth) {
		wstring result;
		int lineWidthCounter=0;
		int len = kanjiList.size();
		for(int i=0;i<len;i++) {
			result.append(1, kanjiList[i]);
			if(lineWidth>0) {
				lineWidthCounter++;
				if(lineWidthCounter>=lineWidth) {
					result.append(1, L'\n');
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
	wstring kanjiStr;
	int grade;

	const BoostHM<wchar_t, KInfo>* ht = KDict::Get()->GetHashTable();

	for(BoostHM<wchar_t,KInfo>::const_iterator
			ki=ht->begin(); ki!=ht->end(); ki++) {
		grade = ki->second.grade;
		if(grade>=lowGrade &&
		  (grade<=highGrade || highGrade==0))
			kanjiStr.append(1, ki->first);
	}

	return AddFromString(kanjiStr);
}

int KanjiList::AddByFrequency(int lowFreq, int highFreq) {
	wstring kanjiStr;
	int freq;

	const BoostHM<wchar_t, KInfo>* ht = KDict::Get()->GetHashTable();

	for(BoostHM<wchar_t,KInfo>::const_iterator ki=ht->begin(); ki!=ht->end(); ki++) {
		freq = ki->second.freq;
		if(freq>=lowFreq && freq<=highFreq)
			kanjiStr.append(1, ki->first);
	}

	return AddFromString(kanjiStr);
}

int KanjiList::Size() {return kanjiList.size();}

void KanjiList::InplaceMerge(vector<wchar_t>& v, BoostHM<wchar_t,int>& indexer, int start, int middle, int end) {
	/* Merge is implemented as a bubble sort started at halfway
	   (since we know the first whole half is already sorted) */
	int i, highIndex;
	wchar_t temp;
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

	myCharIndexer = new BoostHM<wchar_t,int>;
	myCharIndexer->clear();
	vector<wchar_t>::iterator vi;

	/* Create index based on the sort type */
	int value;
	const KDict* kd = KDict::Get();
	const KInfo* ki;
	for(vi=kanjiList.begin();vi!=kanjiList.end();vi++) {
		ki = kd->GetEntry(*vi);
		if(sortType==ST_GRADE)
			value = ki->grade;
		else if(sortType==ST_FREQUENCY)
			value = ki->freq;
		else {
			ostringstream oss;
			oss << "Unknown sort type: " << sortType << endl;
			el.Push(EL_Error, oss.str());
			break;
		}
		if(value==0) value=INT_MAX;
		myCharIndexer->assign(*vi, value);
	}

	/* Sort our data based upon the stored key in the hash table */
	/* This merged sort code was created based upon code at:
	   http://en.wikipedia.org/wiki/Merge_sort#C.2B.2B_implementation
	   These pages were also referred to:
	   http://www.cppreference.com/cppalgorithm/merge.html
	   http://www.cppreference.com/cppalgorithm/inplace_merge.html */
	int rangeSize, rangeStart;
	for(rangeSize=1; rangeSize<totalSize; rangeSize *= 2) {
		for(rangeStart=0;
			rangeStart<totalSize-rangeSize;
			rangeStart += rangeSize*2) {
			/* Our range sort function is HERE */
			InplaceMerge(
			  kanjiList,
			  *myCharIndexer,
			  rangeStart,
			  rangeStart + rangeSize,
			  min(rangeStart + rangeSize*2, totalSize));
		}
	}

	delete myCharIndexer;
}

wchar_t KanjiList::operator[](unsigned int index) {
	if(index<kanjiList.size()) return kanjiList[index];
	return L'\0';
}

int KanjiList::GetIndexByChar(wchar_t c) {
	int i, len = kanjiList.size();
	for(i=0;i<len;i++)
		if(kanjiList[i]==c) return i;
	return -1;
}

vector<wchar_t>& KanjiList::GetVector() {return kanjiList;}
