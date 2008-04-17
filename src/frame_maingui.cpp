/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: frame_maingui.cpp

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

#include "frame_maingui.h"
#include "version.h"
#ifndef __WIN32__
#	include "jben.xpm"
#	include "jben_48.xpm"
#	include "jben_32.xpm"
#	include "jben_16.xpm"
#endif
#include <gtkmm/stock.h>
#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/i18n.h>
#include <boost/format.hpp>
#include "string_utils.h"
#include "preferences.h"

FrameMainGUI* FrameMainGUI::singleton = NULL;

FrameMainGUI& FrameMainGUI::Get() {
	if(!singleton) singleton = new FrameMainGUI;
	return *singleton;
}

void FrameMainGUI::Destroy() {
	if(singleton) {
		delete singleton;
		singleton = NULL;
	}
}

FrameMainGUI::FrameMainGUI() {
	set_title(PROGRAM_NAME);

#ifndef __WIN32__
	/* Load icons */
	list< Glib::RefPtr<Gdk::Pixbuf> > lIcons;
	lIcons.push_back(Gdk::Pixbuf::create_from_xpm_data(iconJben_xpm));
	lIcons.push_back(Gdk::Pixbuf::create_from_xpm_data(iconJben_48_xpm));
	lIcons.push_back(Gdk::Pixbuf::create_from_xpm_data(iconJben_32_xpm));
	lIcons.push_back(Gdk::Pixbuf::create_from_xpm_data(iconJben_16_xpm));
	set_icon_list(lIcons);
#endif

	/* Init vars */
	pdKanjiListEditor = NULL;
	pdVocabListEditor = NULL;
	pdConfig          = NULL;
	pdKanjiPreTest    = NULL;
	pfHWPad           = NULL;
	pfKSearch         = NULL;

	/* Logic copied from widget_storeddialog */
	Preferences* p = Preferences::Get();
	string& size = p->GetSetting("gui.main.size");
	int x, y;
	if(!size.empty()) {
		std::list<string> ls = StrTokenize<char>(size, "x");
		if(ls.size()>=2) {
			x = atoi(ls.front().c_str());
			ls.pop_front();
			y = atoi(ls.front().c_str());
			set_default_size(x,y);
		}
	}

	/* Create menu */
	refActionGroup = Gtk::ActionGroup::create();
	/* File menu */
	refActionGroup->add(Gtk::Action::create("MenuFile", _("_File")));
	refActionGroup->add(
		Gtk::Action::create("MenuFileQuit", Gtk::Stock::QUIT),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuFileQuit));
	/* Edit Menu */
	refActionGroup->add(Gtk::Action::create("MenuEdit", _("_Edit")));
	refActionGroup->add(
		Gtk::Action::create("MenuEditVocab", _("_Vocab Study List")),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuEditVocab));
	refActionGroup->add(
		Gtk::Action::create("MenuEditKanji", _("_Kanji Study List")),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuEditKanji));
	refActionGroup->add(
		Gtk::Action::create("MenuEditPrefs", Gtk::Stock::PREFERENCES),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuEditPrefs));
	/* Practice Menu */
	refActionGroup->add(Gtk::Action::create("MenuPractice", _("_Practice")));
	refActionGroup->add(
		Gtk::Action::create("MenuPracticeKanji", _("_Kanji")),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuPracticeKanji));
	/* Tools Menu */
	refActionGroup->add(Gtk::Action::create("MenuTools", _("_Tools")));
	refActionGroup->add(
		Gtk::Action::create("MenuToolsHand",
							_("_Handwriting Recognition for Kanji")),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuToolsHand));
	refActionGroup->add(
		Gtk::Action::create("MenuToolsKanjiSearch",
							_("_Kanji Search")),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuToolsKanjiSearch));
	/* Help Menu */
	refActionGroup->add(Gtk::Action::create("MenuHelp", _("_Help")));
	refActionGroup->add(
		Gtk::Action::create("MenuHelpAbout", Gtk::Stock::ABOUT),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuHelpAbout));
	refActionGroup->add(
		Gtk::Action::create("MenuHelpLicense", _("_License Information...")),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuHelpLicense));

	refUIManager = Gtk::UIManager::create();
	refUIManager->insert_action_group(refActionGroup);
	add_accel_group(refUIManager->get_accel_group());
	refUIManager->add_ui_from_string(
		"<ui>"
		"  <menubar name='MenuBar'>"
		"    <menu action='MenuFile'>"
		"      <menuitem action='MenuFileQuit'/>"
		"    </menu>"
		"    <menu action='MenuEdit'>"
		"      <menuitem action='MenuEditVocab'/>"
		"      <menuitem action='MenuEditKanji'/>"
		"      <separator/>"
		"      <menuitem action='MenuEditPrefs'/>"
		"    </menu>"
		"    <menu action='MenuPractice'>"
		"      <menuitem action='MenuPracticeKanji'/>"
		"    </menu>"
		"    <menu action='MenuTools'>"
		"      <menuitem action='MenuToolsHand'/>"
		"      <menuitem action='MenuToolsKanjiSearch'/>"
		"    </menu>"
		"    <menu action='MenuHelp'>"
		"      <menuitem action='MenuHelpAbout'/>"
		"      <menuitem action='MenuHelpLicense'/>"
		"    </menu>"
		"  </menubar>"
		"</ui>");

	/* Create window layout */
	add(layoutBox);
	layoutBox.pack_start(*(refUIManager->get_widget("/MenuBar")),
						 Gtk::PACK_SHRINK);
	layoutBox.pack_start(tabs);
	tabs.append_page(panelWordDict, _("Word Dictionary"));
	tabs.append_page(panelKanjiDict, _("Kanji Dictionary"));

	Update();
	show_all_children();
}

FrameMainGUI::~FrameMainGUI() {
	if(pfHWPad)           delete pfHWPad;
	if(pfKSearch)         delete pfKSearch;
	if(pdKanjiListEditor) delete pdKanjiListEditor;
	if(pdVocabListEditor) delete pdVocabListEditor;
	if(pdConfig)          delete pdConfig;
	if(pdKanjiPreTest)    delete pdKanjiPreTest;

	/* Again, this is copied from widget_storeddialog. */
	Preferences* p = Preferences::Get();
	int x, y;
	get_size(x,y);
	string& size = p->GetSetting("gui.main.size");
	size = (boost::format("%dx%d") % x % y).str();
}

void FrameMainGUI::Update() {
	panelWordDict.Update();
	panelKanjiDict.Update();
	if(pfHWPad) pfHWPad->Update();
	if(pfKSearch) pfKSearch->Update();
}

void FrameMainGUI::OnMenuFileQuit() {hide();}

void FrameMainGUI::OnMenuEditVocab() {
	if(!pdVocabListEditor)
		pdVocabListEditor = new DialogVocabListEditor(*this);
	int result = pdVocabListEditor->run();
	pdVocabListEditor->hide();
	if(result==Gtk::RESPONSE_OK)
		panelWordDict.Update();
}

void FrameMainGUI::OnMenuEditKanji() {
	if(!pdKanjiListEditor)
		pdKanjiListEditor = new DialogKanjiListEditor(*this);
	int result = pdKanjiListEditor->run();
	pdKanjiListEditor->hide();
	if(result==Gtk::RESPONSE_OK)
		panelKanjiDict.Update();
}

void FrameMainGUI::OnMenuEditPrefs() {
	if(!pdConfig)
		pdConfig = new DialogConfig(*this);
	int result = pdConfig->run();
	pdConfig->hide();
	if(result==Gtk::RESPONSE_OK)
		Update();
}

void FrameMainGUI::OnMenuPracticeKanji() {
	/* Test config dialog */
	if(!pdKanjiPreTest)
		pdKanjiPreTest = new DialogKanjiPreTest(*this);
	int result = pdKanjiPreTest->run();	
	pdKanjiPreTest->hide();
	if(result==Gtk::RESPONSE_OK) {
		/* Blank out the dictionary panels */
		panelKanjiDict.SetSearchString("");
		panelWordDict .SetSearchString("");
		panelKanjiDict.Update();
		panelWordDict .Update();
		/* Test dialog */
		DialogKanjiTest dkt(*this, *pdKanjiPreTest);
		result = dkt.run();
		dkt.hide();
		/* Result dialog */
		DialogKanjiPostTest dkpt(*this, dkt);
		dkpt.run();
		dkpt.hide();
	}
}

void FrameMainGUI::OnMenuToolsHand() {
	if(!pfHWPad)
		pfHWPad = new FrameHWPad();
	else
		pfHWPad->present();
	pfHWPad->show();
}

void FrameMainGUI::OnMenuToolsKanjiSearch() {
	if(!pfKSearch)
		pfKSearch = new FrameKSearch();
	else
		pfKSearch->present();
	pfKSearch->show();
}

void FrameMainGUI::OnMenuHelpAbout() {
	Gtk::MessageDialog md
		(*this, (boost::format(
			  _("%s v%s\n"
				"By %s\n"
				"Copyright %s\n\n"

				"Inspired in large by JWPce and JFC by Glenn Rosenthal:\n"
				"http://www.physics.ucla.edu/~grosenth/\n\n"

				"Powered by the many Japanese dictionary files from Monash "
				"University, many thanks to Jim Breen:\n"
				"http://www.csse.monash.edu.au/~jwb/japanese.html\n"
				"Thanks also to Jim Rose of kanjicafe.com for his extended "
				"RADKFILE2/KRADFILE2 databases and derived database files.\n\n"

				"Built using gtkmm: http://www.gtkmm.org/\n\n"

				"Hand writing recognition is based upon code from im-ja "
				"(http://im-ja.sourceforge.net/) and KanjiPad "
				"(http://fishsoup.net/software/kanjipadf/).  KanjiPad was "
				"written by Owen Taylor.\n\n"

				"See \"Help->License Information...\" for important license "
				"details."))
		  % PROGRAM_NAME % VERSION_STR % AUTHOR_NAME % COPYRIGHT_DATE).str());
	md.set_title((boost::format(_("About %s")) % PROGRAM_NAME).str());
	md.run();
}

void FrameMainGUI::OnMenuHelpLicense() {
	string licenseMessage(
		_("Program distributed under the GNU General Public License (GPL) "
		  "version 2:\n"
		  "http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt\n\n"

		  "The included dictionary data files, with the exception of the "
		  "handwriting databases and Jim Rose's extensions to the radical "
		  "databases, are distributed under a separate license specified "
		  "at\n"
		  "http://www.csse.monash.edu.au/~jwb/edrdg/license.htm\n\n"

		  "Jim Rose's extended databases are licensed to Monash University "
		  "with permission to modify and redistribute the files, as long as "
		  "his copyright notices are preserved.\n\n"

		  "The SKIP (System of Kanji Indexing by Patterns) system for ordering "
		  "kanji was developed by Jack Halpern (Kanji Dictionary Publishing "
		  "Society at http://www.kanji.org/), and is used with his "
		  "permission.\n\n"

		  "Copies of the GNU General Public License, Monash University's "
		  "license for the dictionary files and documentation for the "
		  "dictionary files are contained in this program's \"license\" "
		  "directory."));
#ifndef __WIN32__
	licenseMessage.append(
		(boost::format(_("  (On this system, it should be located at:\n%s)"))
		 % LICENSEDIR).str());
#endif
	
	Gtk::MessageDialog md(*this, licenseMessage);
	md.set_title(_("License Information"));
	md.run();
}
