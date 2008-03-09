#include "frame_maingui.h"
#include "version.h"
#include "listmanager.h"
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>
#include "jben.xpm"
#include <glibmm/i18n.h>
#include <boost/format.hpp>

#include <iostream>
#include <cstdio>
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
		Gtk::Action::create("MenuEditVocab", _("Vocab Study List")),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuEditVocab));
	refActionGroup->add(
		Gtk::Action::create("MenuEditKanji", _("Kanji Study List")),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuEditKanji));
	refActionGroup->add(
		Gtk::Action::create("MenuEditPrefs", Gtk::Stock::PREFERENCES),
		sigc::mem_fun(*this, &FrameMainGUI::OnMenuEditPrefs));
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

FrameMainGUI::~FrameMainGUI() {}

void FrameMainGUI::OnMenuFileQuit() {hide();}

#if 0
void FrameMainGUI::OnKanjiAddByGrade() {
	cout << "AddGrade" << endl;
	ListManager* lm = ListManager::Get();
	if(!pdAddKanjiByGrade)
		pdAddKanjiByGrade = new DialogAddKanjiByGrade(*this);
	int result = pdAddKanjiByGrade->run();
	pdAddKanjiByGrade->hide();
	if(result==Gtk::RESPONSE_OK) {
		result = lm->KList()->AddByGrade(
			pdAddKanjiByGrade->GetLowGrade(),
			pdAddKanjiByGrade->GetHighGrade());
		//this->Redraw();
		Gtk::MessageDialog md(
			(boost::format(_("Added %d kanji to the list."))
			 % result).str());
		md.set_title(_("Add Kanji by Jouyou Grade"));
		md.run();
	}
	cout << "Result: " << result << endl;
}

void FrameMainGUI::OnKanjiAddByFreq() {
	cout << "AddFreq" << endl;
	ListManager* lm = ListManager::Get();
	if(!pdAddKanjiByFreq)
		pdAddKanjiByFreq = new DialogAddKanjiByFreq(*this);
	int result = pdAddKanjiByFreq->run();
	pdAddKanjiByFreq->hide();
	if(result==Gtk::RESPONSE_OK) {
		result = lm->KList()->AddByFrequency(
			pdAddKanjiByFreq->GetLowFreq(),
			pdAddKanjiByFreq->GetHighFreq());
		//this->Redraw();
		Gtk::MessageDialog md(
			(boost::format(_("Added %d kanji to the list."))
			 % result).str());
		md.set_title(_("Add Kanji by Frequency"));
		md.run();
	}
	cout << "Result: " << result << endl;
}
#endif

void FrameMainGUI::OnMenuEditVocab() {
	cout << "MenuEditVocab" << endl;
}

void FrameMainGUI::OnMenuEditKanji() {
	cout << "MenuEditKanji" << endl;
}

void FrameMainGUI::OnMenuEditPrefs() {
	cout << "MenuEditPrefs" << endl;
}

void FrameMainGUI::OnMenuHelpAbout() {
	cout << "MenuHelpAbout" << endl;
}

void FrameMainGUI::OnMenuHelpLicense() {
	cout << "MenuHelpLicense" << endl;
}

void FrameMainGUI::OnSwitchPage(GtkNotebookPage* page, guint page_num) {
	cout << "OnSwitchPage" << endl;
	((UpdatePanel*)tabs.get_nth_page(page_num))->Update();
}
