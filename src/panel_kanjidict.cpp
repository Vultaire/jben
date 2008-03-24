#include "panel_kanjidict.h"
#include <gtkmm/label.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>
#include <boost/format.hpp>
#include "listmanager.h"
#include "encoding_convert.h"
#include "gtk_utils.h"

PanelKanjiDict::PanelKanjiDict() {
	lblMaxIndex.set_text(_("of 0 kanji"));
	currentIndex = -1;  /* Initialize to a "not selected" index */
	
	btnPrev.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiDict::OnPrevious));
	btnNext.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiDict::OnNext));
	btnRand.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiDict::OnRandom));
	entQuery.signal_activate()
		.connect(sigc::mem_fun(*this, &PanelKanjiDict::OnQueryEnter));
	btnSearch.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelKanjiDict::OnSearch));
	entIndex.signal_activate()
		.connect(sigc::mem_fun(*this, &PanelKanjiDict::OnIndexUpdate));
	tvResults.set_editable(false);
	tvResults.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
	entIndex.set_width_chars(5);
	entIndex.set_max_length(5);
	set_border_width(5);

	Gtk::HBox* phbEntry = manage(new Gtk::HBox(false, 5));
	Gtk::Label* plblSearch
		= manage(new Gtk::Label(_("Enter kanji:")));
	Gtk::ScrolledWindow* pswResults
		= manage(new Gtk::ScrolledWindow);
	Gtk::HBox* phbBottomRow = manage(new Gtk::HBox(false, 5));
	Gtk::HButtonBox* phbbButtons
		= manage(new Gtk::HButtonBox(Gtk::BUTTONBOX_START, 5));

	pack_start(*phbEntry, Gtk::PACK_SHRINK);
	pack_start(*pswResults);
	pack_start(*phbBottomRow, Gtk::PACK_SHRINK);

	phbEntry->pack_start(*plblSearch, Gtk::PACK_SHRINK);
	phbEntry->pack_start(entQuery);
	phbEntry->pack_end(btnSearch, Gtk::PACK_SHRINK);

	pswResults->add(tvResults);
	pswResults->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	pswResults->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	phbBottomRow->pack_start(*phbbButtons, Gtk::PACK_SHRINK);
	phbBottomRow->pack_end(lblMaxIndex, Gtk::PACK_SHRINK);
	phbBottomRow->pack_end(entIndex, Gtk::PACK_SHRINK);

	phbbButtons->pack_start(btnPrev);
	phbbButtons->pack_start(btnNext);
	phbbButtons->pack_start(btnRand);

	show_all_children();
}

void PanelKanjiDict::OnQueryEnter() {
	OnSearch();
}

void PanelKanjiDict::OnSearch() {
	SetSearchString(entQuery.get_text());
	Update();
}

void PanelKanjiDict::OnPrevious() {
	ListManager* lm = ListManager::Get();
	currentIndex--;
	if(currentIndex<0) currentIndex = lm->KList()->Size()-1;
	Update();
}

void PanelKanjiDict::OnNext() {
	ListManager* lm = ListManager::Get();
	int listSize = lm->KList()->Size();
	currentIndex++;
	if(currentIndex>=listSize) currentIndex=0;
	if(listSize==0) currentIndex=-1;
	Update();
}

void PanelKanjiDict::OnRandom() {
	ListManager* lm = ListManager::Get();
	int listSize = lm->KList()->Size();
	if(listSize>0) {
		currentIndex = rand() % listSize;
		Update();
	}
}

void PanelKanjiDict::OnIndexUpdate() {
	ListManager* lm = ListManager::Get();
	string s = entIndex.get_text();
	int i = atoi(s.c_str());	
	if((i>0) && (i <= lm->KList()->Size())) {
		currentIndex = (int)(i-1);
		this->Update();
	} else entIndex.set_text(
		(boost::format("%d") % (currentIndex+1)).str());
}

void PanelKanjiDict::Update() {
	DictPanel::Update();
	/* If currentIndex has been changed, update any necessary data. */
	ListManager* lm = ListManager::Get();
	if(currentIndex!=-1) {
		wchar_t newKanji = (*lm->KList())[currentIndex];
		if(newKanji==L'\0') /* The returned may be 0 if the currentIndex
								  no longer refers to a valid character. */
			currentIndex = -1; /* In this case, we'll reset our index to -1. */
		else if(currentKanji!=newKanji)
			SetSearchString(utfconv_wm(wstring(L"").append(1,newKanji)));
	}

	/* Update most controls */
	entQuery.set_text(currentSearchString);
	entIndex.set_text(
		(boost::format("%d") % (currentIndex+1)).str());
	lblMaxIndex.set_text(
		(boost::format("of %d kanji") % lm->KList()->Size()).str());

	/* Update our output window */
	UpdateOutput();  /* Might want to make this conditionally called
						in the future for performance. */
}

void PanelKanjiDict::SetSearchString(const Glib::ustring& searchString) {
	ListManager* lm = ListManager::Get();
	currentSearchString = searchString;
	int len = currentSearchString.length();
	if(len>1) {
		currentKanji = L'\0';
		currentIndex = -1;
	} else {
		wstring temp = utfconv_mw(searchString);
		currentKanji = temp[0];
		currentIndex = lm->KList()->GetIndexByChar(temp[0]);
	}
}

void PanelKanjiDict::UpdateOutput() {
	const KDict* kd = KDict::Get();
	const KInfo *ki;
	string output;

	wchar_t c;
	int len = currentSearchString.length();
	for(int i=0;i<len;i++) {
		c = currentSearchString[i];
		ki = kd->GetEntry(c);
		if(ki) {
			if(output.length()>0) output.append(1,'\n');
			output.append(KDict::KInfoToTextBuf(*ki));
		}
	}

	if(output.length()==0)
		output.append(_("No kanji have been selected."));
	SetTextBuf(tvResults.get_buffer(), output);
}
