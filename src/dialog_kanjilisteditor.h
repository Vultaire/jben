#ifndef dialog_kanjilisteditor_h
#define dialog_kanjilisteditor_h

#include "widget_storeddialog.h"
#include <gtkmm/button.h>
#include <gtkmm/textview.h>
#include "dialog_addkanjibygrade.h"
#include "dialog_addkanjibyfreq.h"

class DialogKanjiListEditor : public StoredDialog {
public:
	DialogKanjiListEditor(Gtk::Window& parent);
	~DialogKanjiListEditor();
	void Update();
private:
	void OnTextChanged();
	bool ApplyIfNeeded();
	void OnAddFile();
	void OnAddGrade();
	void OnAddFreq();
	void OnSortGrade();
	void OnSortFreq();
	void OnSortBoth();
	void OnCancel();
	void OnApply();
	void OnOK();

	Gtk::TextView tvList;
	Gtk::Button btnAddFile, btnAddGrade, btnAddFreq;
	Gtk::Button btnSortGrade, btnSortFreq, btnSortBoth;
	Gtk::Button btnCancel, btnApply, btnOK;
	bool bChanged;
	DialogAddKanjiByGrade* pdAddByGrade;
	DialogAddKanjiByFreq*  pdAddByFreq;
};

#endif
