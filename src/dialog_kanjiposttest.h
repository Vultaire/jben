#ifndef dialog_kanjiposttest_h
#define dialog_kanjiposttest_h

#include "widget_storeddialog.h"
#include <gtkmm/textview.h>
#include "dialog_kanjitest.h"

class DialogKanjiPostTest : public StoredDialog {
public:
   	DialogKanjiPostTest(Gtk::Window& parent, DialogKanjiTest& test);
private:
	Gtk::TextView tvResults;
};

#endif
