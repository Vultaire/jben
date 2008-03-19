#ifndef dialog_config_h
#define dialog_config_h

#include "widget_storeddialog.h"
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <vector>

class DialogConfig : public StoredDialog {
public:
	DialogConfig(Gtk::Window& parent);
	void Update();
private:
	void OnCancel();
	void OnApply();
	void OnOK();

	void OnDictionaryToggle();

	Gtk::CheckButton chkReadings, chkMeanings, chkHighImp, chkMultiRad, chkDict,
		chkVocabCrossRef, chkLowImp, chkSodStatic, chkSodAnim;
	std::vector<Gtk::CheckButton*> vChkDict;
	Gtk::VBox vbDicts;
	Gtk::Button btnCancel, btnOK;
};

#endif
