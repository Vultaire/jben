#ifndef panel_config_h
#define panel_config_h

#include "widget_updatepanel.h"
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <vector>

class PanelConfig : public UpdatePanel {
public:
	PanelConfig();
	void Update();
private:
	void OnApply();
	void OnDictionaryToggle();
	void OnCheckboxToggle();

	Gtk::CheckButton chkReadings, chkMeanings, chkHighImp, chkMultiRad, chkDict,
		chkVocabCrossRef, chkLowImp, chkSodStatic, chkSodAnim;
	Gtk::Button btnApply;
	std::vector<Gtk::CheckButton*> vChkDict;
};

#endif
