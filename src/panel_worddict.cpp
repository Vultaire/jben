#include "panel_worddict.h"
#include <gtkmm/label.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>
#include <boost/format.hpp>
#include "listmanager.h"
#include "wdict.h"
#include "encoding_convert.h"
#include "gtk_utils.h"

#include <cstdlib>
using namespace std;

PanelWordDict::PanelWordDict() {
	lblMaxIndex.set_text(_("of 0 vocab"));
	currentIndex = -1;  /* Initialize to a "not selected" index */

	btnPrev.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelWordDict::OnPrevious));
	btnNext.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelWordDict::OnNext));
	btnRand.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelWordDict::OnRandom));
	entQuery.signal_activate()
		.connect(sigc::mem_fun(*this, &PanelWordDict::OnQueryEnter));
	btnSearch.signal_clicked()
		.connect(sigc::mem_fun(*this, &PanelWordDict::OnSearch));
	entIndex.signal_activate()
		.connect(sigc::mem_fun(*this, &PanelWordDict::OnIndexUpdate));
	tvResults.set_editable(false);
	tvResults.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
	entIndex.set_width_chars(5);
	entIndex.set_max_length(5);
	set_border_width(5);

	Gtk::HBox* phbEntry = manage(new Gtk::HBox(false, 5));
	Gtk::Label* plblSearch
		= manage(new Gtk::Label(_("Enter word or expression:")));
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

void PanelWordDict::OnQueryEnter() {
	OnSearch();
}

void PanelWordDict::OnSearch() {
	SetSearchString(entQuery.get_text());
	Update();
}

void PanelWordDict::OnPrevious() {
	ListManager* lm = ListManager::Get();
	currentIndex--;
	if(currentIndex<0) currentIndex = lm->VList()->Size()-1;
	Update();
}

void PanelWordDict::OnNext() {
	ListManager* lm = ListManager::Get();
	int listSize = lm->VList()->Size();
	currentIndex++;
	if(currentIndex>=listSize) currentIndex=0;
	if(listSize==0) currentIndex=-1;
	Update();
}

void PanelWordDict::OnRandom() {
	ListManager* lm = ListManager::Get();
	int listSize = lm->VList()->Size();
	if(listSize>0) {
		currentIndex = rand() % listSize;
		Update();
	}
}

void PanelWordDict::OnIndexUpdate() {
	ListManager* lm = ListManager::Get();
	string s = entIndex.get_text();
	int i = atoi(s.c_str());
	if((i>0) && (i <= lm->VList()->Size())) {
		currentIndex = (int)(i-1);
		Update();
	} else entIndex.set_text((boost::format("%d") % (currentIndex+1)).str());
}

void PanelWordDict::SetSearchString(const Glib::ustring& searchString) {
	ListManager* lm = ListManager::Get();
	currentSearchString = searchString;
	currentIndex =
		lm->VList()->GetIndexByWord(utfconv_mw(searchString).c_str());
}

void PanelWordDict::Update() {
	DictPanel::Update();
	/* If currentIndex has been changed, update any necessary data. */
	ListManager* lm = ListManager::Get();
	if(currentIndex!=-1) {
		wstring ws = (*lm->VList())[currentIndex];
		Glib::ustring newVocab;
		if(ws.length()>0) newVocab = utfconv_wm(ws);
		if(newVocab=="")    /* The returned may be 0 if the currentIndex
							   no longer refers to a valid character. */
			currentIndex = -1; /* In this case, we'll reset our index to -1. */
		else if(currentSearchString!=newVocab)
			SetSearchString(newVocab);
	}

	/* Update most controls */
	entQuery.set_text(currentSearchString);
	/* currentIndex is 0-based, so don't forget to adjust it. */
	entIndex.set_text((boost::format("%d") % (currentIndex+1)).str());
	lblMaxIndex.set_text(
		(boost::format(_("of %d vocab")) % lm->VList()->Size()).str());

	/* Update our output window */
	UpdateOutput();  /* Might want to make this conditionally called in
						the future for performance. */
}

void PanelWordDict::UpdateOutput() {
	list<int> resultList;
	const WDict *wd = WDict::Get();

	string output;
	if(currentSearchString.length()==0) {
		output = _("No search has been entered.");
	} else {
		/* Get search results string */
		if(wd->Search(utfconv_mw(currentSearchString).c_str(), resultList)) {
			/* Create merged wx-compatible results string */
			string resultString, temp;
			for(list<int>::iterator li = resultList.begin();
			  li!=resultList.end();
			  li++) {
				if(resultString.length()!=0) resultString.append(1, '\n');
				temp = wd->GetEdictString(*li);
				resultString.append(temp);
			}
			/* Convert search results to destination format */
			output = WDict::ResultToTextBuf(resultString);
		} else {
			output = _("No results were found.");
		}
	}

	SetTextBuf(tvResults.get_buffer(), output);
}
