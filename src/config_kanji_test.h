/*
  Project: J-Ben
  Website: http://www.vultaire.net/software/jben/
  License: GNU General Public License (GPL) version 2
  (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

  File:    config_kanji_test.h
  Author:  Alain BERTRAND

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

#ifndef config_kanji_test_h
#define config_kanji_test_h

#include "widget_storeddialog.h"
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/table.h>
#include <gtkmm/textview.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include<gtkmm/entry.h>
#include <vector>
#include <string>

class ConfigKanjiTest : public Gtk::VBox {
public:
  ConfigKanjiTest();
  ~ConfigKanjiTest();
  /** sets the preference map to the new values, called by
      DialogConfig::OnOk*/
  void validate();
  
protected :
  /** return "true" or "false" has string depending on value*/
  std::string boolString(bool value);
  // kw : kanji write
  Gtk::CheckButton *kwShowOnYomi;
  Gtk::CheckButton *kwShowKunYomi;
  Gtk::CheckButton *kwShowEnglish;
  //kr : kanji read
  Gtk::CheckButton *krShowOnYomi;
  Gtk::CheckButton *krShowKunYomi;
  Gtk::CheckButton *krShowEnglish;
  //short cut definitions
  Gtk::Entry *okField;
  Gtk::Entry *wrongField;
  Gtk::Entry *showAnswerField;
  Gtk::Entry *stopField;

};

#endif
