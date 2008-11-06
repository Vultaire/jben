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
#include <gtkmm/label.h>
#include <glibmm/i18n.h>
#include "config_kanji_test.h"
#include "preferences.h"

ConfigKanjiTest::ConfigKanjiTest():Gtk::VBox(1,3){

  //upframe : the frame for the kanjiwrite settings
  Gtk::Frame *upFrame = manage(new Gtk::Frame("Write kanji test"));
  pack_start(*upFrame);

  //upbox : item to show or not in kanji write test

  Gtk::VBox* upBox= manage(new Gtk::VBox);
  upFrame->add(*upBox);

  // create check buttons

  kwShowOnYomi=manage(new Gtk::CheckButton(_("Show on-yomi")));
  kwShowKunYomi=manage(new Gtk::CheckButton(_("Show kun-yomi")));
  kwShowEnglish=manage(new Gtk::CheckButton(_("Show english meaning")));

  //put the buttons in the upbox

  upBox->pack_start(*kwShowOnYomi);
  upBox->pack_start(*kwShowKunYomi);
  upBox->pack_start(*kwShowEnglish);

  // set their states according to the preferences

  Preferences* pref=Preferences::Get();
  kwShowOnYomi->set_active(pref->GetSetting("kanjitest.kanjiwrite.showonyomi")=="true");
  kwShowKunYomi->set_active(pref->GetSetting("kanjitest.kanjiwrite.showkunyomi")=="true");
  kwShowEnglish->set_active(pref->GetSetting("kanjitest.kanjiwrite.showenglish")=="true"); 


//midframe : the frame for the kanji read settings

  Gtk::Frame *midFrame = manage(new Gtk::Frame("Read kanji test"));
  pack_start(*midFrame);

  //midbox : item to show or not in kanji read test

  Gtk::VBox* midBox= manage(new Gtk::VBox);
  midFrame->add(*midBox);

  // create check buttons

  krShowOnYomi=manage(new Gtk::CheckButton(_("Show on-yomi")));
  krShowKunYomi=manage(new Gtk::CheckButton(_("Show kun-yomi")));
  krShowEnglish=manage(new Gtk::CheckButton(_("Show english meaning")));

  //put the buttons in the midbox

  midBox->pack_start(*krShowOnYomi);
  midBox->pack_start(*krShowKunYomi);
  midBox->pack_start(*krShowEnglish);

  // set their states according to their preferences

  krShowOnYomi->set_active(pref->GetSetting("kanjitest.kanjiread.showonyomi")=="true");
  krShowKunYomi->set_active(pref->GetSetting("kanjitest.kanjiread.showkunyomi")=="true");
  krShowEnglish->set_active(pref->GetSetting("kanjitest.kanjiread.showenglish")=="true");

  //botframe : the frame for the short cuts settings

  Gtk::Frame *botFrame = manage(new Gtk::Frame("Keyboard shortcuts"));
  pack_start(*botFrame);

  //botbox : item to show or not in kanji read test

  Gtk::Table* botBox= manage(new Gtk::Table(2,5));
  botBox->set_col_spacings(10);
  botFrame->add(*botBox);
  Gtk::Label *lab = manage(new Gtk::Label(_("Correct answer : "),Gtk::ALIGN_LEFT ));
  botBox->attach(*lab,0, 1, 0, 1);

  okField= manage (new Gtk::Entry);
  okField->set_text(pref->GetSetting("kanjitest.correctanswer"));
  okField->set_max_length(1);
  okField->set_size_request(30);
  botBox->attach(*okField,1, 2, 0, 1,Gtk::SHRINK);

  lab= manage(new Gtk::Label(_("Wrong answer : "),Gtk::ALIGN_LEFT));
  botBox->attach(*lab,2, 3, 0, 1);

  wrongField= manage (new Gtk::Entry);
  wrongField->set_text(pref->GetSetting("kanjitest.wronganswer"));
  wrongField->set_max_length(1);
  wrongField->set_size_request(30);
  botBox->attach(*wrongField,3, 4, 0, 1,Gtk::SHRINK);


  lab= manage(new Gtk::Label(_("Show answer : "),Gtk::ALIGN_LEFT));
  botBox->attach(*lab,0, 1, 1, 2);

  showAnswerField= manage (new Gtk::Entry);
  showAnswerField->set_text(pref->GetSetting("kanjitest.showanswer"));
  showAnswerField->set_max_length(1);
  showAnswerField->set_size_request(30);
  botBox->attach(*showAnswerField,1, 2, 1, 2,Gtk::SHRINK);

  lab= manage(new Gtk::Label(_("Stop drill : "),Gtk::ALIGN_LEFT));
  botBox->attach(*lab,2, 3, 1, 2);

  stopField= manage (new Gtk::Entry);
  stopField->set_text(pref->GetSetting("kanjitest.stopdrill"));
  stopField->set_max_length(1);
  stopField->set_size_request(30);
  botBox->attach(*stopField,3, 4, 1, 2,Gtk::SHRINK);


  
}

ConfigKanjiTest::~ConfigKanjiTest(){
}

void ConfigKanjiTest::validate(){

  Preferences* pref=Preferences::Get();

  pref->GetSetting("kanjitest.kanjiwrite.showonyomi")=boolString(kwShowOnYomi->get_active());
  pref->GetSetting("kanjitest.kanjiwrite.showkunyomi")=boolString(kwShowKunYomi->get_active());
  pref->GetSetting("kanjitest.kanjiwrite.showenglish")=boolString(kwShowEnglish->get_active());


  pref->GetSetting("kanjitest.kanjiread.showonyomi")=boolString(krShowOnYomi->get_active());
  pref->GetSetting("kanjitest.kanjiread.showkunyomi")=boolString(krShowKunYomi->get_active());
  pref->GetSetting("kanjitest.kanjiread.showenglish")=boolString(krShowEnglish->get_active());

  pref->GetSetting("kanjitest.correctanswer")=okField->get_text();
  pref->GetSetting("kanjitest.wronganswer")=wrongField->get_text();
  pref->GetSetting("kanjitest.showanswer")=showAnswerField->get_text();
  pref->GetSetting("kanjitest.stopdrill")=stopField->get_text();
}
std::string  ConfigKanjiTest::boolString(bool value){
  std::string result="false";
  if (value){
    result="true";
  }
  return result;
}
