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

#include "kdict.h"
#include "boosthm.h"
#include <string>
#include <vector>
using namespace std;

#define ST_GRADE     1
#define ST_FREQUENCY 2

class KanjiList {
public:
	KanjiList(const BoostHM<wchar_t, string>* const kDictHash);
	int AddFromString(const wstring& s);
	int AddByGrade(int lowGrade, int highGrade);
	int AddByFrequency(int lowFreq, int highFreq);
	wstring ToString(int lineWidth = 0);
	int Size();
	void Clear();
	void Sort(int sortType, bool reverseOrder=false);
	wchar_t operator[](unsigned int index);
	int GetIndexByChar(wchar_t c);
	vector<wchar_t>& GetVector();

private:
	void InplaceMerge(vector<wchar_t>& v, BoostHM<wchar_t, int>& indexer, int start, int middle, int end);

	const BoostHM<wchar_t, string> *kanjiHash;
	vector<wchar_t> kanjiList;
	BoostHM<wchar_t, int> *myCharIndexer;
};

#endif
