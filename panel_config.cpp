/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_config.cpp

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

#include "panel_config.h"
#include "kanjidic.h"
#include "global.h"

enum {
	ID_chkReadings=1,
	ID_chkMeanings,
	ID_chkHighImportance,
	ID_chkVocabCrossRef,
	ID_chkLowImportance,
	ID_chkUseSODs,
	ID_chkUseSODAs,

	ID_scrlDictionaries,
	ID_btnApply,

	ID_chkDictionaries,
	ID_chkDictBase
};

BEGIN_EVENT_TABLE(PanelConfig, wxPanel)
	EVT_BUTTON(ID_btnApply, PanelConfig::OnApply)
	EVT_CHECKBOX(ID_chkDictionaries, PanelConfig::OnDictionaryToggle)
END_EVENT_TABLE()

PanelConfig::PanelConfig(wxWindow *owner) : RedrawablePanel(owner) {
	/* Make main controls */
	chkReadings = new wxCheckBox(this, ID_chkReadings, _T("Include on-yomi, kun-yomi, and nanori (name) readings"));
	chkMeanings = new wxCheckBox(this, ID_chkMeanings, _T("Include English meanings"));
	chkHighImportance = new wxCheckBox(this, ID_chkHighImportance, _T("Include stroke count, Jouyou grade and newspaper frequency rank"));
	chkDictionaries = new wxCheckBox(this, ID_chkDictionaries, _T("Include dictionary reference codes"));
	scrlDictionaries = new wxScrolledWindow(this, ID_scrlDictionaries, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL | wxSUNKEN_BORDER | wxTAB_TRAVERSAL);
	scrlDictionaries->SetBackgroundColour(*wxWHITE);
	chkVocabCrossRef = new wxCheckBox(this, ID_chkVocabCrossRef, _T("Show vocab from your study list which use this kanji"));
	chkLowImportance = new wxCheckBox(this, ID_chkLowImportance, _T("Other information (Radical #'s, Korean and Pinyin romanization)"));
	chkUseSODs = new wxCheckBox(this, ID_chkUseSODs, _T("Use KanjiCafe.com stroke order diagrams"));
	chkUseSODAs = new wxCheckBox(this, ID_chkUseSODAs, _T("Use KanjiCafe.com animated stroke order diagrams"));
	wxButton *btnApply = new wxButton(this, ID_btnApply, _T("Apply Changes"));

	/* Create our list of dictionaries */
	dictionaryList.Add(_T("\"New Japanese-English Character Dictionary\" by Jack Halpern."));
	dictionaryList.Add(_T("\"Modern Reader's Japanese-English Character Dictionary\" by Andrew Nelson"));
	dictionaryList.Add(_T("\"The New Nelson Japanese-English Character Dictionary\" by John Haig"));
	dictionaryList.Add(_T("\"Japanese for Busy People\" by AJLT"));
	dictionaryList.Add(_T("\"The Kanji Way to Japanese Language Power\" by Dale Crowley"));
	dictionaryList.Add(_T("\"Kodansha Compact Kanji Guide\""));
	dictionaryList.Add(_T("\"A Guide To Reading and Writing Japanese\" by Ken Hensall et al."));
	dictionaryList.Add(_T("\"Kanji in Context\" by Nishiguchi and Kono"));
	dictionaryList.Add(_T("\"Kanji Learner's Dictionary\" by Jack Halpern"));
	dictionaryList.Add(_T("\"Essential Kanji\" by P.G. O'Neill"));
	dictionaryList.Add(_T("\"2001 Kanji\" by Father Joseph De Roo"));
	dictionaryList.Add(_T("\"A Guide To Reading and Writing Japanese\" by Florence Sakade"));
	dictionaryList.Add(_T("\"Tuttle Kanji Cards\" by Alexander Kask"));
	dictionaryList.Add(_T("\"Japanese Kanji Flashcards\" by White Rabbit Press"));
	dictionaryList.Add(_T("(Not yet supported) SKIP codes used by Halpern dictionaries"));
	dictionaryList.Add(_T("\"Kanji Dictionary\" by Spahn && Hadamitzky"));
	dictionaryList.Add(_T("\"Kanji && Kana\" by Spahn && Hadamitzky"));
	dictionaryList.Add(_T("Four Corner code (various dictionaries)"));
	dictionaryList.Add(_T("\"Morohashi Daikanwajiten\" (index number)"));
	dictionaryList.Add(_T("\"Morohashi Daikanwajiten\" (volume.page number)"));
	dictionaryList.Add(_T("\"A Guide to Remembering Japanese Characters\" by Kenneth G. Henshal"));
	dictionaryList.Add(_T("Gakken Kanji Dictionary (\"A New Dictionary of Kanji Usage\")"));
	dictionaryList.Add(_T("\"Remembering The Kanji\" by James Heisig"));
	dictionaryList.Add(_T("\"Japanese Names\" by P.G. O'Neill"));

	/* Make controls for dictionary stuff */
	int dictCount = dictionaryList.Count();
	if(dictCount>0)	chkarrayDictionaries = new ScrolledCheck*[dictCount];
	else chkarrayDictionaries = NULL;
	wxBoxSizer *dictSizer = new wxBoxSizer(wxVERTICAL);
	int i;
	for(i=0;i<dictCount;i++) {
		chkarrayDictionaries[i] = new ScrolledCheck(scrlDictionaries, ID_chkDictBase+i, i, dictionaryList[i]);
		dictSizer->Add(chkarrayDictionaries[i]);
	}
	if(dictCount>0)
		scrlDictionaries->SetScrollRate(25, chkarrayDictionaries[0]->GetSize().GetHeight());
	scrlDictionaries->SetSizer(dictSizer);

	/* Connect event handlers for all checkboxes (except the parent dictionary checkbox,
	   since it already has a special handler configured.) */
	/* NOTE:
	   Here's the Connect call used in the Event sample:
	       Connect(Event_Dynamic, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
           (wxObjectEventFunction)
           (wxEventFunction)
           (wxCommandEventFunction)&MyFrame::OnDynamic);
	   In practice, only the (wxObjectEventFunction) cast appears to be necessary (under GNU gcc). */
	this->Connect(ID_chkReadings, ID_chkUseSODAs,
		wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&PanelConfig::OnCheckboxToggle);
	this->Connect(ID_chkDictBase, ID_chkDictBase+dictCount-1,
		wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&PanelConfig::OnCheckboxToggle);

	/* Add everything to the main panel */
	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	panelSizer->Add(chkReadings, 0, wxTOP | wxLEFT | wxRIGHT, 10);
	panelSizer->Add(chkMeanings, 0, wxLEFT | wxRIGHT, 10);
	panelSizer->Add(chkHighImportance, 0, wxLEFT | wxRIGHT, 10);
	panelSizer->Add(chkDictionaries, 0, wxTOP | wxLEFT | wxRIGHT, 10);
	panelSizer->Add(scrlDictionaries, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
	panelSizer->Add(chkVocabCrossRef, 0, wxLEFT | wxRIGHT, 10);
	panelSizer->Add(chkLowImportance, 0, wxLEFT | wxRIGHT, 10);
	panelSizer->Add(chkUseSODs, 0, wxLEFT | wxRIGHT, 10);
	panelSizer->Add(chkUseSODAs, 0, wxLEFT | wxRIGHT, 10);
	panelSizer->Add(btnApply, 0, wxALIGN_CENTER | wxALL, 10);

	changeDetected = false;
	processingRedraw = false;
	dictionariesEnabled = jben->prefs->kanjidicOptions & KDO_DICTIONARIES;
	UpdateDictionaryControls(dictionariesEnabled);
	this->SetSizerAndFit(panelSizer);
}

PanelConfig::~PanelConfig() {
	if(chkarrayDictionaries) delete[] chkarrayDictionaries;
}

void PanelConfig::OnApply(wxCommandEvent& event) {
	Commit();
}

void PanelConfig::OnCheckboxToggle(wxCommandEvent& event) {
	if(!processingRedraw) {
		changeDetected = true;
	}
}

void PanelConfig::OnDictionaryToggle(wxCommandEvent& event) {
	if(!processingRedraw) {
		bool value = chkDictionaries->GetValue();
		if(value!=dictionariesEnabled) {
			dictionariesEnabled = value;
			UpdateDictionaryControls(dictionariesEnabled);
			OnCheckboxToggle(event);
		}
	}
}

void PanelConfig::Commit() {
	changeDetected = false;
	unsigned long options=0, dictionaries=0;
	if(chkReadings->GetValue()) options |= KDO_READINGS;
	if(chkMeanings->GetValue()) options |= KDO_MEANINGS;
	if(chkHighImportance->GetValue()) options |= KDO_HIGHIMPORTANCE;
	if(chkDictionaries->GetValue()) options |= KDO_DICTIONARIES;
	if(chkVocabCrossRef->GetValue()) options |= KDO_VOCABCROSSREF;
	if(chkLowImportance->GetValue()) options |= KDO_LOWIMPORTANCE;
	if(chkUseSODs->GetValue()) options |= KDO_SOD_STATIC;
	if(chkUseSODAs->GetValue()) options |= KDO_SOD_ANIM;

	int dictCount = dictionaryList.Count();
	for(int i=0; i<dictCount; i++) {
		if(chkarrayDictionaries[i]->GetValue()) dictionaries |= (1ul << i);
	}
	jben->prefs->kanjidicOptions = options;
	jben->prefs->kanjidicDictionaries = dictionaries;
}

void PanelConfig::Revert() {
	changeDetected = false;
	/* We could have a Redraw() call here, but currently Revert is only called when changing away from this panel,
	   and thus it's a pointless call since Redraw will be done when this panel is selected again. */
}

void PanelConfig::Redraw() {
	processingRedraw = true;
	int options = jben->prefs->kanjidicOptions;
	int dictionaries = jben->prefs->kanjidicDictionaries;

	/* Set appropriate checkboxes */
	chkReadings->SetValue(options & KDO_READINGS);
	chkMeanings->SetValue(options & KDO_MEANINGS);
	chkHighImportance->SetValue(options & KDO_HIGHIMPORTANCE);
	chkDictionaries->SetValue(options & KDO_DICTIONARIES);
	chkVocabCrossRef->SetValue(options & KDO_VOCABCROSSREF);
	chkLowImportance->SetValue(options & KDO_LOWIMPORTANCE);
	chkUseSODs->SetValue(options & KDO_SOD_STATIC);
	chkUseSODAs->SetValue(options & KDO_SOD_ANIM);

	int dictCount = dictionaryList.Count();
	for(int i=0;i<dictCount;i++) {
		chkarrayDictionaries[i]->SetValue(dictionaries & (1ul << i));
	}

	scrlDictionaries->Scroll(0,0);

	processingRedraw = false;
	dictionariesEnabled = chkDictionaries->GetValue();
}

bool PanelConfig::ChangeDetected() {return changeDetected;}

void PanelConfig::UpdateDictionaryControls(bool state) {
	if(state) {
		scrlDictionaries->Enable();
	} else {
		scrlDictionaries->Disable();
	}
}
