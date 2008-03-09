#ifndef panel_kanjidict_h
#define panel_kanjidict_h

#include "widget_updatepanel.h"
#include <gtkmm/entry.h>
#include <gtkmm/textview.h>
#include <gtkmm/button.h>

class PanelKanjiDict : public UpdatePanel {
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

	Gtk::Entry entQuery;
	Gtk::Button btnSearch;
	Gtk::TextView tvResults;
	Gtk::Button btnPrev, btnNext, btnRand;
	Gtk::Entry entIndex;
	Gtk::Label lblMaxIndex;
};

#endif
