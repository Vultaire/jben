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
