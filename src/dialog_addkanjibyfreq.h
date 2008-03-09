#ifndef addkanjibyfreq_h
#define addkanjibyfreq_h

#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>

class DialogAddKanjiByFreq : public Gtk::Dialog {
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
