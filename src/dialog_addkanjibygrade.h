#ifndef addkanjibygrade_h
#define addkanjibygrade_h

#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>

class DialogAddKanjiByGrade : public Gtk::Dialog {
public:
	DialogAddKanjiByGrade(Gtk::Window& parent);
	int GetLowGrade();
	int GetHighGrade();
private:
	void OnOK();
	void OnCancel();
	void OnLowValChange();
	void OnHighValChange();

	Gtk::ComboBoxText comboLowGrade, comboHighGrade;
	Gtk::Button btnOK, btnCancel;
	void OKProc();
	void CancelProc();
};

#endif
