/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: vocablist.h

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

#ifndef vocablist_h
#define vocablist_h

#include <string>
#include <vector>
using namespace std;

#define ST_GRADE     1
#define ST_FREQUENCY 2

class VocabList {
public:
	VocabList();
	bool Add(const wstring& s);
	int AddList(const wstring& s);
	wstring ToString(wchar_t separator=L'\n');
	int Size();
	void Clear();
	const wstring& operator[](unsigned int index);
	int GetIndexByWord(const wstring& s);
	vector<wstring>& GetVocabList();
private:
	vector<wstring> vocabList;
	int BinarySearch(const wstring& query, bool* matchFound);
};

#endif
