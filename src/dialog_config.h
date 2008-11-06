/*
Project: J-Ben
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File:         dialog_config.h
Author:       Paul Goins
Contributors: Alain Bertrand

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef dialog_config_h
#define dialog_config_h

#include "config_kanji_test.h"
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
	/* Kanji Test Configuration */
	ConfigKanjiTest ckt;
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
