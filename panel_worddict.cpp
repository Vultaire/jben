/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_worddict.cpp

This file is part of J-Ben.

J-Ben is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

J-Ben is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "panel_worddict.h"
#include "global.h"
#include "jutils.h"

enum {
	ID_textSearch=1,
	ID_btnPrev,
	ID_btnNext,
	ID_btnRand,
	ID_textIndex
};

BEGIN_EVENT_TABLE(PanelWordDict, wxPanel)
	EVT_TEXT_ENTER(ID_textSearch, PanelWordDict::OnSearch)
	EVT_BUTTON(ID_btnPrev, PanelWordDict::OnPrevious)
	EVT_BUTTON(ID_btnNext, PanelWordDict::OnNext)
	EVT_BUTTON(ID_btnRand, PanelWordDict::OnRandom)
	EVT_TEXT_ENTER(ID_textIndex, PanelWordDict::OnIndexUpdate)
END_EVENT_TABLE()

PanelWordDict::PanelWordDict(wxWindow *owner) : RedrawablePanel(owner) {
	/* searchBox controls */
	wxStaticText *labelSearch = new wxStaticText(this,-1,_T("Enter word or expression:"));
	textSearch = new wxTextCtrl(this, ID_textSearch, _T(""),
		wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	/* Our output window */
	htmlOutput = new wxHtmlWindow(this, -1,
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	/* navBox controls */
	wxButton *btnPrev = new wxButton(this, ID_btnPrev, _T("<< Previous"));
	wxButton *btnNext = new wxButton(this, ID_btnNext, _T("Next >>"));
	wxButton *btnRand = new wxButton(this, ID_btnRand, _T("Random"));
	/*btnRand->Enable(false);*/
	textIndex = new wxTextCtrl(this, ID_textIndex, _T("0"),
		wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	textIndex->SetMaxLength(4);
	lblIndex = new wxStaticText(this, wxID_ANY, _T(" of 0 vocab"));

	/* searchBox: Add both components horizonally, vertically centered,
	   and space of 10px between them.  No outer spacing; this'll be
	   handled in the parent sizer. */
	wxBoxSizer *searchBox = new wxBoxSizer(wxHORIZONTAL);
	searchBox->Add(labelSearch, 0, wxALIGN_CENTER_VERTICAL);
	searchBox->Add(textSearch, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL
		| wxLEFT, 10);

	/* navBox:  */
	wxBoxSizer *navBox = new wxBoxSizer(wxHORIZONTAL);
	navBox->Add(btnPrev, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	navBox->Add(btnNext, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	navBox->Add(btnRand, 0, wxALIGN_CENTER_VERTICAL);
	navBox->AddStretchSpacer(1);
	navBox->Add(textIndex, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	navBox->Add(lblIndex, 0, wxALIGN_CENTER_VERTICAL);

	/* windowBox: 10px around/between all controls. */
	wxBoxSizer *windowBox = new wxBoxSizer(wxVERTICAL);
	windowBox->Add(searchBox, 0, wxEXPAND
		| wxLEFT | wxTOP | wxRIGHT, 10);
	windowBox->Add(htmlOutput, 1, wxEXPAND
		| wxALL, 10);
	windowBox->Add(navBox, 0, wxEXPAND
		| wxLEFT | wxBOTTOM | wxRIGHT, 10);

	/* Other initialization */
	currentIndex = -1; /* Currently selected vocab (none) is not in the list, so set to -1 */
	UpdateHtmlOutput(); /* Do an initial update of the output window */

	/* Finally: Apply window sizer and fit window */
	this->SetSizerAndFit(windowBox);

}

void PanelWordDict::OnSearch(wxCommandEvent& event) {
	SetSearchString(textSearch->GetValue());
	this->Redraw();
}

void PanelWordDict::SetSearchString(const wxString& searchString) {
	currentSearchString = searchString;
	currentIndex = jben->vocabList->GetIndexByWord(searchString);
}

void PanelWordDict::Redraw() {
	/* If currentIndex has been changed, update any necessary data. */
	if(currentIndex!=-1) {
		wxString newVocab = (*jben->vocabList)[currentIndex];
		if(newVocab==_T(""))    /* The returned may be 0 if the currentIndex no longer refers to a valid character. */
			currentIndex = -1;    /* In this case, we'll reset our index to -1. */
		else if(currentSearchString!=newVocab)
			SetSearchString(newVocab);
	}

	/* Update most controls */
	textSearch->ChangeValue(currentSearchString);
	textIndex->ChangeValue(wxString::Format(_T("%d"), currentIndex+1));
	lblIndex->SetLabel(wxString::Format(_T(" of %d vocab"), jben->vocabList->Size()));
	/* We need to tell our sizer to refresh to accomodate the resizing of the label. */
	this->GetSizer()->Layout();

	/* Update our output window */
	UpdateHtmlOutput();  /* Might want to make this conditionally called in the future for performance. */
}

void PanelWordDict::UpdateHtmlOutput() {
	list<int> resultList;

	wxString html = _T("<html><body><font face=\"Serif\">");
	if(currentSearchString.length()==0) {
		html.append(_T("No search has been entered."));
	} else {
		/* Get search results string */
		if(jben->edict->Search(currentSearchString, resultList)) {
			/* Create merged wx-compatible results string */
			wxString resultString, temp;
			for(list<int>::iterator li = resultList.begin();
			  li!=resultList.end();
			  li++) {
				if(resultString.length()!=0) resultString.append(_T('\n'));
				EUCToWx(jben->edict->GetEdictString(*li), temp);
				resultString.append(temp);
			}
			/* Convert search results to destination format
			For now: HTML
			Later: wxWidgets Rich Text */
			resultString = jben->edict->ResultToHTML(resultString);
			html.append(resultString);
		} else {
			html.append(_T("No results were found."));
		}
	}
	html.append(_T("</font></body></html>"));

	htmlOutput->SetPage(html);
}

void PanelWordDict::OnPrevious(wxCommandEvent& event) {
	currentIndex--;
	if(currentIndex<0) currentIndex = jben->vocabList->Size()-1;

	this->Redraw();
}

void PanelWordDict::OnNext(wxCommandEvent& event) {
	int listSize = jben->vocabList->Size();
	currentIndex++;
	if(currentIndex>=listSize) currentIndex=0;
	if(listSize==0) currentIndex=-1;
	this->Redraw();
}

/* Very quick and dirty pseudorandom jump */
void PanelWordDict::OnRandom(wxCommandEvent& event) {
	int listSize = jben->vocabList->Size();
	if(listSize>0) {
		currentIndex = rand() % listSize;
		this->Redraw();
	}
}

void PanelWordDict::OnIndexUpdate(wxCommandEvent& event) {
	long l;
	wxString str = textIndex->GetValue();
	if(str.ToLong(&l) && (l>0) && (l <= jben->vocabList->Size())) {
		currentIndex = (int)(l-1);
		this->Redraw();
	} else textIndex->ChangeValue(wxString::Format(_T("%d"), currentIndex+1));
}
