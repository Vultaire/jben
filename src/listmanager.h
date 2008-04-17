/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: listmanager.h

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

#ifndef listmanager_h
#define listmanager_h

#include "vocablist.h"
#include "kanjilist.h"
#include <map>

class ListManager {
public:
	static ListManager* Get();
	static void Destroy();
	static bool Exists();

	void AddKanjiList(const string& name);
	void AddVocabList(const string& name);
	bool RemoveKanjiList(const string& name);
	bool RemoveVocabList(const string& name);
	bool ChooseKanjiList(const string& name);
	bool ChooseVocabList(const string& name);

	KanjiList* KList();
	VocabList* VList();
private:
	static ListManager* singleton;
	ListManager();

	map<string, KanjiList> kanjiList;
	map<string, VocabList> vocabList;
	string currentKanjiList, currentVocabList;
};

#endif
