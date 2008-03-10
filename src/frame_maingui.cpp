#include "frame_maingui.h"
#include "version.h"
#include "jben.xpm"
#include <gtkmm/stock.h>
#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/i18n.h>
#include <boost/format.hpp>

#include <iostream>
using namespace std;

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
	set_title(PROGRAM_NAME " v" VERSION_STR " by " AUTHOR_NAME);
	set_default_size(600,400);
	Glib::RefPtr<Gdk::Pixbuf> rpIcon
		= Gdk::Pixbuf::create_from_xpm_data(iconJben_xpm);
	set_icon(rpIcon);

	pdKanjiListEditor = NULL;
	pdVocabListEditor = NULL;
	pdConfig          = NULL;
	pdKanjiPreTest    = NULL;

	/* General event handlers */
	tabs.signal_switch_page().connect(
		sigc::mem_fun(*this, &FrameMainGUI::OnSwitchPage));

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

	show_all_children();
}

FrameMainGUI::~FrameMainGUI() {
	if(pdKanjiListEditor) delete pdKanjiListEditor;
	if(pdVocabListEditor) delete pdVocabListEditor;
	if(pdConfig) delete pdConfig;
	if(pdKanjiPreTest) delete pdKanjiPreTest;
}

void FrameMainGUI::OnMenuFileQuit() {hide();}

void FrameMainGUI::OnMenuEditVocab() {
	cout << "MenuEditVocab" << endl;
	if(!pdVocabListEditor)
		pdVocabListEditor = new DialogVocabListEditor(*this);
	int result = pdVocabListEditor->run();
	pdVocabListEditor->hide();
	if(result==Gtk::RESPONSE_OK) {
		/* DO NOTHING - handled by the dialog itself */
	}
}

void FrameMainGUI::OnMenuEditKanji() {
	cout << "MenuEditKanji" << endl;
	if(!pdKanjiListEditor)
		pdKanjiListEditor = new DialogKanjiListEditor(*this);
	int result = pdKanjiListEditor->run();
	pdKanjiListEditor->hide();
	if(result==Gtk::RESPONSE_OK) {
		/* DO NOTHING - handled by the dialog itself */
	}
}

void FrameMainGUI::OnMenuEditPrefs() {
	cout << "MenuEditPrefs" << endl;
	if(!pdConfig)
		pdConfig = new DialogConfig(*this);
	int result = pdConfig->run();
	pdConfig->hide();
	if(result==Gtk::RESPONSE_OK) {
		/* DO NOTHING - handled by the dialog itself */
	}
}

void FrameMainGUI::OnMenuPracticeKanji() {
	cout << "MenuPracticeKanji" << endl;
	if(!pdKanjiPreTest)
		pdKanjiPreTest = new DialogKanjiPreTest(*this);
	int result = pdKanjiPreTest->run();	
	pdKanjiPreTest->hide();
	if(result==Gtk::RESPONSE_OK) {
		DialogKanjiTest dkt(*this, *pdKanjiPreTest);
		result = dkt.run();
		dkt.hide();
		/* Show result dialog - to do! */
		DialogKanjiPostTest dkpt(*this, dkt);
		dkpt.run();
	}
}

void FrameMainGUI::OnMenuHelpAbout() {
	cout << "MenuHelpAbout" << endl;
	Gtk::MessageDialog md
		(*this, (boost::format(
			  _("%s  v%s\n"
				"By %s\n"
				"Copyright %s\n\n"

				"Inspired in large by JWPce and JFC by Glenn Rosenthal:\n"
				"http://www.physics.ucla.edu/~grosenth/\n\n"

				"Powered by Monash University's EDICT2 and KANJIDIC:\n"
				"http://www.csse.monash.edu.au/~jwb/edict_doc.html\n"
				"http://www.csse.monash.edu.au/~jwb/kanjidic.html\n\n"

				"Built using gtkmm: http://www.gtkmm.org/\n\n"

				"Hand writing recognition is based upon code from im-ja:\n"
				"http://im-ja.sourceforge.net/\n"
				"Which uses code based upon KanjiPad by Owen Taylor:\n"
				"http://fishsoup.net/software/kanjipad/\n\n"

				"See \"Help->License Information...\" for important license "
				"details."))
		  % PROGRAM_NAME % VERSION_STR % AUTHOR_NAME % COPYRIGHT_DATE).str());
	md.set_title((boost::format(_("About %s")) % PROGRAM_NAME).str());
	md.run();
}

void FrameMainGUI::OnMenuHelpLicense() {
	cout << "MenuHelpLicense" << endl;
	string licenseMessage(
		_("Program distributed under the GNU General Public License (GPL) "
		  "version 2:\n"
		  "http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt\n\n"
		  "EDICT2 and KANJIDIC distributed under a separate license specified "
		  "at\n"
		  "http://www.csse.monash.edu.au/~jwb/edrdg/license.htm\n\n"

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

void FrameMainGUI::OnSwitchPage(GtkNotebookPage* page, guint page_num) {
	cout << "OnSwitchPage" << endl;
	((UpdatePanel*)tabs.get_nth_page(page_num))->Update();
}
