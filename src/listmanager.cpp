/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: listmanager.cpp

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

#include "listmanager.h"

ListManager* ListManager::singleton = NULL;

ListManager* ListManager::Get() {
	if(!singleton) singleton = new ListManager();
	return singleton;
}

void ListManager::Destroy() {
	if(singleton) delete singleton;
	singleton = NULL;
}

ListManager::ListManager() {}

/* Add kanji/vocab lists if they are not already present.
   Do nothing if they are. */
void ListManager::AddKanjiList(const string& name) {kanjiList[name];}
void ListManager::AddVocabList(const string& name) {vocabList[name];}

bool ListManager::RemoveKanjiList(const string& name) {
	map<string,KanjiList>::iterator it;
	it = kanjiList.find(name);
	if(it==kanjiList.end()) return false;

	kanjiList.erase(it);
	if(currentKanjiList==name) currentKanjiList="";
	return true;
}
bool ListManager::RemoveVocabList(const string& name) {
	map<string,VocabList>::iterator it;
	it = vocabList.find(name);
	if(it==vocabList.end()) return false;

	vocabList.erase(it);
	if(currentVocabList==name) currentVocabList="";
	return true;
}

bool ListManager::ChooseKanjiList(const string& name) {
	if(name=="") {
		currentKanjiList = name;
		return true;
	}
	map<string,KanjiList>::iterator it
		= kanjiList.find(name);
	if(it==kanjiList.end()) return false;
	currentKanjiList = name;
	return true;
}

bool ListManager::ChooseVocabList(const string& name) {
	if(name=="") {
		currentVocabList = name;
		return true;
	}
	map<string,VocabList>::iterator it
		= vocabList.find(name);
	if(it==vocabList.end()) return false;
	currentVocabList = name;
	return true;
}

KanjiList* ListManager::KList() {
	map<string,KanjiList>::iterator it;
	it = kanjiList.find(currentKanjiList);
	if(it==kanjiList.end()) return NULL;
	return &(it->second);
}

VocabList* ListManager::VList() {
	map<string,VocabList>::iterator it;
	it = vocabList.find(currentVocabList);
	if(it==vocabList.end()) return NULL;
	return &(it->second);
}

bool ListManager::Exists() {
	return (singleton!=NULL);
}
