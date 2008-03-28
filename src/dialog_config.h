#ifndef dialog_config_h
#define dialog_config_h

#include "widget_storeddialog.h"
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/table.h>
#include <gtkmm/textview.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <vector>
#include <string>

class DialogConfig : public StoredDialog {
public:
	DialogConfig(Gtk::Window& parent);
	void Update();
	void UpdateFontControl(Gtk::TextView& tvEnNormal,
		const std::string& sFontEnNormal);
private:
	void OnCancel();
	void OnApply();
	void OnOK();

	void OnDictionaryToggle();
	void OnFontChange(Gtk::Button* src);
	void OnMobileToggle();

	/* Kanji Dictionary Configuration */
	Gtk::CheckButton chkReadings, chkMeanings, chkHighImp, chkMultiRad, chkDict,
		chkVocabCrossRef, chkLowImp, chkSodStatic, chkSodAnim;
	std::vector<Gtk::CheckButton*> vChkDict;
	Gtk::VBox vbDicts;
	/* Font Config */
	Gtk::Table tblFonts;
	Gtk::Frame    frJaNormal,  frJaLarge,  frEnNormal,  frEnSmall;
	Gtk::TextView tvJaNormal,  tvJaLarge,  tvEnNormal,  tvEnSmall;
	Gtk::Label   lblJaNormal, lblJaLarge, lblEnNormal, lblEnSmall;
	Gtk::Button  btnJaNormal, btnJaLarge, btnEnNormal, btnEnSmall;
	std::string sFontJaNormal, sFontJaLarge, sFontEnNormal, sFontEnSmall;
	/* Other Options */
	Gtk::CheckButton chkMobile;
	/* Buttons */
	Gtk::Button btnCancel, btnOK;
};

#endif
