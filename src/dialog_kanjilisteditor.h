#ifndef dialog_kanjilisteditor_h
#define dialog_kanjilisteditor_h

#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>

class DialogKanjiListEditor : public Gtk::Dialog {
public:
	DialogKanjiListEditor(Gtk::Window& parent);
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
