#ifndef panel_worddict_h
#define panel_worddict_h

#include "widget_updatepanel.h"
#include <gtkmm/entry.h>
#include <gtkmm/textview.h>
#include <gtkmm/button.h>
#include <glibmm/ustring.h>

class PanelWordDict : public UpdatePanel {
public:
	PanelWordDict();
	void Update();
private:
	void UpdateOutput();
	void SetSearchString(const Glib::ustring& searchString);

	int currentIndex;
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
