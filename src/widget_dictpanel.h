#ifndef widget_dictpanel_h
#define widget_dictpanel_h

#include "widget_updatepanel.h"
#include <gtkmm/textview.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/texttag.h>

class DictPanel : public UpdatePanel {
public:
	DictPanel();
	virtual void Update();
protected:
	Gtk::TextView tvResults;
	Gtk::Entry entQuery;
	Gtk::Button btnSearch;
	Gtk::Button btnPrev, btnNext, btnRand;
	Gtk::Entry entIndex;
	Gtk::Label lblMaxIndex;
};

#endif
