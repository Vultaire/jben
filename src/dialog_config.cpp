#include "dialog_config.h"
#include "preferences.h"
#include "kdict.h"
#include "file_utils.h"
#include "jben_defines.h"
#include <glibmm/i18n.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/stock.h>
#include <gtkmm/fontselection.h>
#include <gtkmm/messagedialog.h>
#include <boost/format.hpp>

#define CHOOSE_FONT _("Change...")

DialogConfig::DialogConfig(Gtk::Window& parent)
	: StoredDialog(_("Preferences Editor"), parent, "gui.dlg.preferences.size"),
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
	  tblFonts(4, 3),
	  lblJaNormal(_("Japanese Font, Normal")),
	  lblJaLarge (_("Japanese Font, Large")),
	  lblEnNormal(_("English Font, Normal")),
	  lblEnSmall (_("English Font, Small")),
	  btnJaNormal(CHOOSE_FONT),
	  btnJaLarge (CHOOSE_FONT),
	  btnEnNormal(CHOOSE_FONT),
	  btnEnSmall (CHOOSE_FONT),
	  chkMobile(_("Mobile mode (settings saved to current directory)")),
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
		new Gtk::CheckButton(_("\"Kanji Dictionary\" by Spahn & Hadamitzky")));
	vChkDict.push_back(pc);
	pc = manage(
		new Gtk::CheckButton(_("\"Kanji & Kana\" by Spahn & Hadamitzky")));
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
	Gtk::Notebook *pnb = manage(new Gtk::Notebook);
	Gtk::VBox *pvbKanjiConfig, *pvbFontConfig;
	Gtk::VBox *pvbKanjiMainOpts, *pvbKanjiDictsOuter, *pvbKanjiOtherOpts;
	pvbKanjiConfig = manage(new Gtk::VBox);
	pvbKanjiMainOpts   =  manage(new Gtk::VBox);
	pvbKanjiDictsOuter =  manage(new Gtk::VBox);
	pvbKanjiOtherOpts  =  manage(new Gtk::VBox);
	pvbFontConfig  = manage(new Gtk::VBox);
	Gtk::HButtonBox *phbbButtons = manage(new Gtk::HButtonBox);
	Gtk::ScrolledWindow *pswDicts = manage(new Gtk::ScrolledWindow);
	pswDicts->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	pvbKanjiConfig->pack_start(*pvbKanjiMainOpts,   Gtk::PACK_SHRINK);
	pvbKanjiConfig->pack_start(*pvbKanjiDictsOuter, Gtk::PACK_EXPAND_WIDGET);
	pvbKanjiConfig->pack_start(*pvbKanjiOtherOpts,  Gtk::PACK_SHRINK);
	pvbKanjiConfig->pack_start(*phbbButtons,   Gtk::PACK_SHRINK);

	pvbKanjiMainOpts->pack_start(chkReadings, Gtk::PACK_SHRINK);
	pvbKanjiMainOpts->pack_start(chkMeanings, Gtk::PACK_SHRINK);
	pvbKanjiMainOpts->pack_start(chkHighImp,  Gtk::PACK_SHRINK);
	pvbKanjiMainOpts->pack_start(chkMultiRad, Gtk::PACK_SHRINK);

	pvbKanjiDictsOuter->pack_start(chkDict,   Gtk::PACK_SHRINK);
	pvbKanjiDictsOuter->pack_start(*pswDicts, Gtk::PACK_EXPAND_WIDGET);

	pswDicts->add(vbDicts);
	for(it = vChkDict.begin(); it != vChkDict.end(); it++) {
		vbDicts.pack_start(**it, Gtk::PACK_SHRINK);
	}
	vbDicts.set_sensitive(false);

	pvbKanjiOtherOpts->pack_start(chkVocabCrossRef, Gtk::PACK_SHRINK);
	pvbKanjiOtherOpts->pack_start(chkLowImp, Gtk::PACK_SHRINK);
	pvbKanjiOtherOpts->pack_start(chkSodStatic, Gtk::PACK_SHRINK);
	pvbKanjiOtherOpts->pack_start(chkSodAnim, Gtk::PACK_SHRINK);

	pvbFontConfig->set_spacing(5);
	pvbFontConfig->pack_start(tblFonts, Gtk::PACK_SHRINK);

	frJaNormal.add(tvJaNormal);
	frJaNormal.set_shadow_type(Gtk::SHADOW_IN);
	frJaLarge .add(tvJaLarge);
	frJaLarge .set_shadow_type(Gtk::SHADOW_IN);
	frEnNormal.add(tvEnNormal);
	frEnNormal.set_shadow_type(Gtk::SHADOW_IN);
	frEnSmall .add(tvEnSmall);
	frEnSmall .set_shadow_type(Gtk::SHADOW_IN);
	tvJaNormal.set_accepts_tab(false);
	tvJaLarge .set_accepts_tab(false);
	tvEnNormal.set_accepts_tab(false);
	tvEnSmall .set_accepts_tab(false);
	tvJaNormal.set_editable(false);
	tvJaLarge .set_editable(false);
	tvEnNormal.set_editable(false);
	tvEnSmall .set_editable(false);

	Gtk::HButtonBox *fntBtnBox1 = manage(new Gtk::HButtonBox);
	fntBtnBox1->pack_start(btnJaNormal, Gtk::PACK_SHRINK);
	Gtk::HButtonBox *fntBtnBox2 = manage(new Gtk::HButtonBox);
	fntBtnBox2->pack_start(btnJaLarge, Gtk::PACK_SHRINK);
	Gtk::HButtonBox *fntBtnBox3 = manage(new Gtk::HButtonBox);
	fntBtnBox3->pack_start(btnEnNormal, Gtk::PACK_SHRINK);
	Gtk::HButtonBox *fntBtnBox4 = manage(new Gtk::HButtonBox);
	fntBtnBox4->pack_start(btnEnSmall, Gtk::PACK_SHRINK);
	
	tblFonts.attach(lblJaNormal, 0, 1, 0, 1, Gtk::SHRINK,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(frJaNormal,  1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(*fntBtnBox1, 2, 3, 0, 1, Gtk::SHRINK,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(lblJaLarge,  0, 1, 1, 2, Gtk::SHRINK,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(frJaLarge,   1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(*fntBtnBox2,  2, 3, 1, 2, Gtk::SHRINK,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(lblEnNormal, 0, 1, 2, 3, Gtk::SHRINK,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(frEnNormal,  1, 2, 2, 3, Gtk::FILL | Gtk::EXPAND,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(*fntBtnBox3, 2, 3, 2, 3, Gtk::SHRINK,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(lblEnSmall,  0, 1, 3, 4, Gtk::SHRINK,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(frEnSmall,   1, 2, 3, 4, Gtk::FILL | Gtk::EXPAND,
		Gtk::SHRINK, 5, 5);
	tblFonts.attach(*fntBtnBox4,  2, 3, 3, 4, Gtk::SHRINK,
		Gtk::SHRINK, 5, 5);

	btnJaNormal.signal_clicked().connect(sigc::bind<Gtk::Button*>(
		sigc::mem_fun(*this, &DialogConfig::OnFontChange), &btnJaNormal));
	btnJaLarge.signal_clicked() .connect(sigc::bind<Gtk::Button*>(
		sigc::mem_fun(*this, &DialogConfig::OnFontChange), &btnJaLarge));
	btnEnNormal.signal_clicked().connect(sigc::bind<Gtk::Button*>(
		sigc::mem_fun(*this, &DialogConfig::OnFontChange), &btnEnNormal));
	btnEnSmall.signal_clicked() .connect(sigc::bind<Gtk::Button*>(
		sigc::mem_fun(*this, &DialogConfig::OnFontChange), &btnEnSmall));

	pnb->append_page(*pvbKanjiConfig, _("Kanji Dictionary"));
	pnb->append_page(*pvbFontConfig, _("Fonts"));

	/* Windows only: "mobile mode" */
#ifdef __WIN32__
	chkMobile.signal_toggled()
		.connect(sigc::mem_fun(*this, &DialogConfig::OnMobileToggle));
	Gtk::VBox *pvbOtherConfig = manage(new Gtk::VBox);
	pvbOtherConfig->pack_start(chkMobile, Gtk::PACK_SHRINK);
	pnb->append_page(*pvbOtherConfig, _("Other"));
#endif

	set_border_width(5);
	Gtk::VBox* pvb = get_vbox();
	pvb->set_spacing(5);
	pvb->pack_start(*pnb);

	Gtk::HButtonBox* phbb = get_action_area();
	phbb->pack_start(btnCancel);
	phbb->pack_start(btnOK);

	Update();
	show_all_children();
}

void DialogConfig::OnDictionaryToggle() {
	vbDicts.set_sensitive(chkDict.get_active());
}

void DialogConfig::OnCancel() {
	Update();
	response(Gtk::RESPONSE_CANCEL);
}

void DialogConfig::OnOK() {
	unsigned long options=0, dictionaries=0;
	if(chkReadings     .get_active()) options |= KDO_READINGS;
	if(chkMeanings     .get_active()) options |= KDO_MEANINGS;
	if(chkHighImp      .get_active()) options |= KDO_HIGHIMPORTANCE;
	if(chkMultiRad     .get_active()) options |= KDO_MULTIRAD;
	if(chkDict         .get_active()) options |= KDO_DICTIONARIES;
	if(chkVocabCrossRef.get_active()) options |= KDO_VOCABCROSSREF;
	if(chkLowImp       .get_active()) options |= KDO_LOWIMPORTANCE;
	if(chkSodStatic    .get_active()) options |= KDO_SOD_STATIC;
	if(chkSodAnim      .get_active()) options |= KDO_SOD_ANIM;

	for(size_t i=0; i<vChkDict.size(); i++) {
		if(vChkDict[i]->get_active()) dictionaries |= (1ul << i);
	}
	Preferences *prefs = Preferences::Get();
	prefs->kanjidicOptions = options;
	prefs->kanjidicDictionaries = dictionaries;
	prefs->GetSetting("font.ja")       = sFontJaNormal;
	prefs->GetSetting("font.ja.large") = sFontJaLarge;
	prefs->GetSetting("font.en")       = sFontEnNormal;
	prefs->GetSetting("font.en.small") = sFontEnSmall;

	if(chkMobile.get_active())
		prefs->GetSetting("config_save_target") = "mobile";
	else
		prefs->GetSetting("config_save_target") = "home";

	Update();  /* Probably unnecessary, but let's be safe. */
	response(Gtk::RESPONSE_OK);
}

void DialogConfig::Update() {
	Preferences *prefs = Preferences::Get();
	int options = prefs->kanjidicOptions;
	int dictionaries = prefs->kanjidicDictionaries;

	/* Set appropriate checkboxes */
	chkReadings     .set_active(options & KDO_READINGS);
	chkMeanings     .set_active(options & KDO_MEANINGS);
	chkHighImp      .set_active(options & KDO_HIGHIMPORTANCE);
	chkMultiRad     .set_active(options & KDO_MULTIRAD);
	chkDict         .set_active(options & KDO_DICTIONARIES);
	chkVocabCrossRef.set_active(options & KDO_VOCABCROSSREF);
	chkLowImp       .set_active(options & KDO_LOWIMPORTANCE);
	chkSodStatic    .set_active(options & KDO_SOD_STATIC);
	chkSodAnim      .set_active(options & KDO_SOD_ANIM);

	/* Enable/disable SOD/SODA checkboxes based on existance of directory */
	string sodDir = Preferences::Get()->GetSetting("sod_dir");
	chkSodStatic.set_sensitive(
		FileExists(
			string(sodDir)
			.append(1,DSCHAR)
			.append("sod-utf8-hex")
			.append(1,DSCHAR)
			.append("README-License").c_str()));
	chkSodAnim.set_sensitive(
		FileExists(
			string(sodDir)
			.append(1,DSCHAR)
			.append("soda-utf8-hex")
			.append(1,DSCHAR)
			.append("README-License").c_str()));

	for(size_t i=0;i<vChkDict.size();i++) {
		vChkDict[i]->set_active(dictionaries & (1ul << i));
	}

	/* Init font display */
	sFontJaNormal = prefs->GetSetting("font.ja");
	sFontJaLarge  = prefs->GetSetting("font.ja.large");
	sFontEnNormal = prefs->GetSetting("font.en");
	sFontEnSmall  = prefs->GetSetting("font.en.small");
	UpdateFontControl(tvJaNormal, sFontJaNormal);
	UpdateFontControl(tvJaLarge,  sFontJaLarge);
	UpdateFontControl(tvEnNormal, sFontEnNormal);
	UpdateFontControl(tvEnSmall,  sFontEnSmall);

	/* Other Options */
	chkMobile.set_active(prefs->GetSetting("config_save_target") == "mobile");
}

void DialogConfig::UpdateFontControl(Gtk::TextView& tv, const string& font) {
	Glib::RefPtr<Gtk::TextBuffer>   pBuf = tv.get_buffer();
	Glib::RefPtr<Gtk::TextTagTable> pTable = pBuf->get_tag_table();
	Glib::RefPtr<Gtk::TextTag> pTag;
	pTag = pTable->lookup("font");
	if(pTag!=0) {
		pTable->remove(pTag);
		pTag.clear();
	}
	pTag = Gtk::TextTag::create("font");
	pTag->property_font() = font;
	pTable->add(pTag);
	pBuf->set_text("");
	if(&tv == &tvJaNormal || &tv == &tvJaLarge) {
		pBuf->insert_with_tag(pBuf->begin(), "日本語", pTag);
	} else {
		pBuf->insert_with_tag(pBuf->begin(), "English", pTag);
	}
}

void DialogConfig::OnFontChange(Gtk::Button* src) {
	Gtk::FontSelectionDialog fd(_("Choose Font"));
	Gtk::TextView* ptv;
	string* ps;
	if(src==&btnJaNormal) {
		ptv = &tvJaNormal;
		ps  = &sFontJaNormal;
		fd.set_preview_text("ENGLISH english 日本語　にほんご　ニホンゴ");
	} else if(src==&btnJaLarge) {
		ptv = &tvJaLarge;
		ps  = &sFontJaLarge;
		fd.set_preview_text("ENGLISH english 日本語　にほんご　ニホンゴ");
	} else if(src==&btnEnNormal) {
		ptv = &tvEnNormal;
		ps  = &sFontEnNormal;
	} else if(src==&btnEnSmall) {
		ptv = &tvEnSmall;
		ps  = &sFontEnSmall;
	} else return; /* Should not happen */

	fd.set_font_name(*ps);
	int result = fd.run();
	if(result == Gtk::RESPONSE_OK) {
		(*ps) = fd.get_font_name();
		UpdateFontControl(*ptv, *ps);
	}
}

void DialogConfig::OnMobileToggle() {
	if(chkMobile.get_active()==false) {
		Preferences *p = Preferences::Get();
		if(p->GetSetting("config_save_target")=="mobile") {
			/* The user is attempting a change from a mobile to standard
			   install.  We need to check whether a standard install config
			   file is already present, and if it is, we need to warn the
			   user before they accidentally overwrite it. */
			string sCfgPath;
			char *sz = getenv(HOMEENV);
			if(sz) {
				sCfgPath = sz;
				sCfgPath.append(1, DSCHAR);
				sCfgPath.append(CFGFILE);
			} else return; /* No home dir is a problem of its own, but at least
							  the user won't lose data over the problem. */
			if(FileExists(sCfgPath.c_str())) {
				Gtk::MessageDialog md(
					(boost::format(_(
						"A config file was found in this system's home folder: "
						"%s.  Turning mobile mode off will cause J-Ben to "
						"OVERWRITE the existing file when it closes, "
						"destroying whatever is already there.  Is this okay?"))
						% sCfgPath).str(),
					false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
				int result = md.run();
				if(result == Gtk::RESPONSE_NO) {
					chkMobile.set_active(true);
				}
			}
		}
	}
}
