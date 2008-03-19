#ifndef dialog_addkanjibyfreq_h
#define dialog_addkanjibyfreq_h

#include "widget_storeddialog.h"
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>

class DialogAddKanjiByFreq : public StoredDialog {
public:
	DialogAddKanjiByFreq(Gtk::Window& parent);
	int GetLowFreq();
	int GetHighFreq();
private:
	void OnOK();
	void OnCancel();
	void OnLowValChange();
	void OnHighValChange();

	Gtk::SpinButton spinLowFreq, spinHighFreq;
	Gtk::Button btnOK, btnCancel;
	void OKProc();
	void CancelProc();
};

#endif
