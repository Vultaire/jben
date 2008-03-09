#ifndef dialog_config_h
#define dialog_config_h

#include <gtkmm/dialog.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <vector>

class DialogConfig : public Gtk::Dialog {
public:
	DialogConfig(Gtk::Window& parent);
private:
	void OnCancel();
	void OnApply();
	void OnOK();

	void OnDictionaryToggle();

	Gtk::CheckButton chkReadings, chkMeanings, chkHighImp, chkMultiRad, chkDict,
		chkVocabCrossRef, chkLowImp, chkSodStatic, chkSodAnim;
	std::vector<Gtk::CheckButton*> vChkDict;
	Gtk::Button btnCancel, btnOK;
};

#endif
