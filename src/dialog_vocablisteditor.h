#ifndef dialog_vocablisteditor_h
#define dialog_vocablisteditor_h

#include "widget_storeddialog.h"
#include <gtkmm/button.h>
#include <gtkmm/textview.h>

class DialogVocabListEditor : public StoredDialog {
public:
	DialogVocabListEditor(Gtk::Window& parent);
	void Update();
private:
	void OnTextChanged();
	void OnCancel();
	void OnApply();
	void OnOK();

	Gtk::TextView tvList;
	Gtk::Button btnCancel, btnApply, btnOK;
	bool bChanged;
};

#endif
