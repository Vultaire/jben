#ifndef dialog_kanjipretest_h
#define dialog_kanjipretest_h

#include "widget_storeddialog.h"
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>

class DialogKanjiPreTest : public StoredDialog {
public:
   	DialogKanjiPreTest(Gtk::Window& parent);
	Gtk::RadioButton rdoRandom, rdoIndex, rdoReading, rdoWriting;
	Gtk::SpinButton spnCount, spnIndex;
private:
	void OnKanjiCountChange();
	void OnRdoRandom();
	void OnRdoStartIndex();
	void OnCancel();
	void OnStart();

	Gtk::Button btnCancel, btnStart;
};

#endif
