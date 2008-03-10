#ifndef dialog_kanjiposttest_h
#define dialog_kanjiposttest_h

#include <gtkmm/dialog.h>
#include <gtkmm/textview.h>
#include "dialog_kanjitest.h"

class DialogKanjiPostTest : public Gtk::Dialog {
public:
   	DialogKanjiPostTest(Gtk::Window& parent, DialogKanjiTest& test);
private:
	Gtk::TextView tvResults;
};

#endif
