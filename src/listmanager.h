#ifndef listmanager_h
#define listmanager_h

#include "vocablist.h"
#include "kanjilist.h"

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
