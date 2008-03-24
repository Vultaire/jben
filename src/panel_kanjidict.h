#ifndef panel_kanjidict_h
#define panel_kanjidict_h

#include "widget_dictpanel.h"

class PanelKanjiDict : public DictPanel {
public:
	PanelKanjiDict();
	void Update();
private:
	void UpdateOutput();
	void SetSearchString(const Glib::ustring& searchString);

	int currentIndex;
	wchar_t currentKanji;
	Glib::ustring currentSearchString;

	void OnQueryEnter();
	void OnSearch();
	void OnPrevious();
	void OnNext();
	void OnRandom();
	void OnIndexUpdate();
};

#endif
