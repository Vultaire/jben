#ifndef dialog_addkanjibygrade_h
#define dialog_addkanjibygrade_h

#include "widget_storeddialog.h"
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>

class DialogAddKanjiByGrade : public StoredDialog {
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
