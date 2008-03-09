#include "dialog_config.h"
#include <glibmm/i18n.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/stock.h>

#include <iostream>
using namespace std;

DialogConfig::DialogConfig(Gtk::Window& parent)
	: Dialog(_("Preferences Editor"), parent, true),
	  chkReadings(_("Include on-yomi, kun-yomi and nanori (name) readings")),
	  chkMeanings(_("Include English meanings")),
	  chkHighImp(_("Include stroke count, Jouyou grade "
				   "and newspaper frequency rank")),
	  chkMultiRad(_("Include radical component list")),
	  chkDict(_("Include dictionary reference codes")),
	  chkVocabCrossRef(_("Show vocab from your study list "
						 "which use the kanji")),
	  chkLowImp(_("Other information (Radical #'s, "
				  "Korean and Pinyin romanization)")),
	  chkSodStatic(_("Use KanjiCafe.com stroke order diagrams")),
	  chkSodAnim(_("Use KanjiCafe.com animated stroke order diagrams")),
	  btnCancel(Gtk::Stock::CANCEL),
	  btnOK(Gtk::Stock::OK)
{
	std::vector<Gtk::CheckButton*>::iterator it;

	/* Create list of dictionary checkbuttons */
	Gtk::CheckButton* pc;
	pc = manage(
		new Gtk::CheckButton(_("\"New Japanese-English Character Dictionary\" "
							   "by Jack Halpern.")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Modern Reader's Japanese-English "
							   "Character Dictionary\" by Andrew Nelson")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"The New Nelson Japanese-English "
							   "Character Dictionary\" by John Haig")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Japanese for Busy People\" by AJLT")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"The Kanji Way to Japanese Language Power\" "
							   "by Dale Crowley")));
	vChkDict.push_back(pc);
	pc = manage(new Gtk::CheckButton(_("\"Kodansha Compact Kanji Guide\"")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"A Guide To Reading and Writing Japanese\" "
							   "by Ken Hensall et al.")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Kanji in Context\" by Nishiguchi and Kono")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Kanji Learner's Dictionary\" "
							   "by Jack Halpern")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Essential Kanji\" by P.G. O'Neill")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"2001 Kanji\" by Father Joseph De Roo")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"A Guide To Reading and Writing Japanese\" "
							   "by Florence Sakade")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Tuttle Kanji Cards\" by Alexander Kask")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Japanese Kanji Flashcards\" "
							   "by White Rabbit Press")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("SKIP codes used by Halpern dictionaries")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Kanji Dictionary\" by Spahn && Hadamitzky")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Kanji && Kana\" by Spahn && Hadamitzky")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("Four Corner code (various dictionaries)")));
	vChkDict.push_back(pc);
	pc = manage(new Gtk::CheckButton(_("\"Morohashi Daikanwajiten\"")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"A Guide to Remembering Japanese Characters\" "
							   "by Kenneth G. Henshal")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("Gakken Kanji Dictionary "
							   "(\"A New Dictionary of Kanji Usage\")")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Remembering The Kanji\" by James Heisig")));
	vChkDict.push_back(pc);
	pc = manage(new Gtk::CheckButton(_("\"Japanese Names\" by P.G. O'Neill")));

	/* Connect signals */
	chkDict.signal_toggled()
		.connect(sigc::mem_fun(*this, &DialogConfig::OnDictionaryToggle));
	btnCancel.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogConfig::OnCancel));
	btnOK.signal_clicked()
		.connect(sigc::mem_fun(*this, &DialogConfig::OnOK));

	/* Layout and display */
	Gtk::VBox *pvbMainOpts, *pvbDictsOuter, *pvbDictsInner, *pvbOtherOpts;
	pvbMainOpts =   manage(new Gtk::VBox);
	pvbDictsOuter = manage(new Gtk::VBox);
	pvbDictsInner = manage(new Gtk::VBox);
	pvbOtherOpts =  manage(new Gtk::VBox);
	Gtk::HButtonBox *phbbButtons = manage(new Gtk::HButtonBox);
	Gtk::ScrolledWindow *pswDicts = manage(new Gtk::ScrolledWindow);
	pswDicts->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	Gtk::VBox* pvb = get_vbox();
	pvb->set_spacing(5);
	pvb->pack_start(*pvbMainOpts,   Gtk::PACK_SHRINK);
	pvb->pack_start(*pvbDictsOuter, Gtk::PACK_EXPAND_WIDGET);
	pvb->pack_start(*pvbOtherOpts,  Gtk::PACK_SHRINK);
	pvb->pack_start(*phbbButtons,   Gtk::PACK_SHRINK);

	pvbMainOpts->pack_start(chkReadings, Gtk::PACK_SHRINK);
	pvbMainOpts->pack_start(chkMeanings, Gtk::PACK_SHRINK);
	pvbMainOpts->pack_start(chkHighImp,  Gtk::PACK_SHRINK);
	pvbMainOpts->pack_start(chkMultiRad, Gtk::PACK_SHRINK);

	pvbDictsOuter->pack_start(chkDict,   Gtk::PACK_SHRINK);
	pvbDictsOuter->pack_start(*pswDicts, Gtk::PACK_EXPAND_WIDGET);

	pswDicts->add(*pvbDictsInner);
	for(it = vChkDict.begin(); it != vChkDict.end(); it++) {
		pvbDictsInner->pack_start(**it, Gtk::PACK_SHRINK);
	}

	pvbOtherOpts->pack_start(chkVocabCrossRef, Gtk::PACK_SHRINK);
	pvbOtherOpts->pack_start(chkLowImp, Gtk::PACK_SHRINK);
	pvbOtherOpts->pack_start(chkSodStatic, Gtk::PACK_SHRINK);
	pvbOtherOpts->pack_start(chkSodAnim, Gtk::PACK_SHRINK);

	Gtk::HButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnOK);

	show_all_children();
}


void DialogConfig::OnDictionaryToggle() {
	cout << "OnDictionaryToggle" << endl;
}

void DialogConfig::OnCancel() {
	cout << "Cancel" << endl;
	response(Gtk::RESPONSE_CANCEL);
}

void DialogConfig::OnOK() {
	cout << "OK" << endl;
	/* DO STUFF */
	response(Gtk::RESPONSE_OK);
}
