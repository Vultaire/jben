/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_vocablisteditor.cpp

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

#include "panel_vocablisteditor.h"
#include "jben.h"

enum {
	ID_btnRevert=1,
	ID_btnCommit,
	ID_textVocabList
};

BEGIN_EVENT_TABLE(PanelVocabListEditor, wxPanel)
	EVT_BUTTON(ID_btnRevert, PanelVocabListEditor::OnRevert)
	EVT_BUTTON(ID_btnCommit, PanelVocabListEditor::OnCommit)
	EVT_TEXT(ID_textVocabList, PanelVocabListEditor::OnTextChanged)
END_EVENT_TABLE()

PanelVocabListEditor::PanelVocabListEditor(wxWindow *owner) : RedrawablePanel(owner) {
	textVocabList = new wxTextCtrl(this, ID_textVocabList, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxButton *btnRevert = new wxButton(this, ID_btnRevert, _T("Revert"));
	wxButton *btnCommit = new wxButton(this, ID_btnCommit, _T("Commit"));

	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	buttonSizer->AddStretchSpacer(1);
	buttonSizer->Add(btnRevert);
	buttonSizer->AddStretchSpacer(1);
	buttonSizer->Add(btnCommit);
	buttonSizer->AddStretchSpacer(1);
	panelSizer->Add(textVocabList, 1, wxEXPAND | wxALL, 10);
	panelSizer->Add(buttonSizer, 0, wxEXPAND | wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10);

	changeDetected = false;
	this->SetSizerAndFit(panelSizer);
}

void PanelVocabListEditor::Commit() {
	/* This function, when called explicitly, caused a hard lock and
	   LOTS of HDD activity.  Step through this with GDB, and monitor
	   mem usage with top. */
	changeDetected = false;
	jben->vocabList->Clear();
	int result = jben->vocabList->AddList(textVocabList->GetValue());

	jben->gui->Redraw();

	Redraw();	/* Might be redundant now with the above Redraw() call... */
	wxMessageBox(wxString::Format(_T("Changes Saved.\nTotal vocab in list: %d"), result),
				_T("Vocab List Editor"), wxOK | wxICON_INFORMATION, this);
}

void PanelVocabListEditor::Revert() {
	changeDetected = false;
	Redraw();
}

void PanelVocabListEditor::OnRevert(wxCommandEvent& ev) {Revert();}
void PanelVocabListEditor::OnCommit(wxCommandEvent& ev) {Commit();}

void PanelVocabListEditor::OnTextChanged(wxCommandEvent& ev) {
	changeDetected = true;
}

bool PanelVocabListEditor::ChangeDetected() {return changeDetected;}

void PanelVocabListEditor::Redraw() {
	/*textVocabList->ChangeValue(jben->vocabList->ToString());*/
	wxString s = jben->vocabList->ToString();
	textVocabList->ChangeValue(s);
	textVocabList->ChangeValue(s);
	textVocabList->ChangeValue(s);
	textVocabList->ChangeValue(s);
	textVocabList->ChangeValue(s);
	textVocabList->ChangeValue(s);
	textVocabList->ChangeValue(s);
	textVocabList->ChangeValue(s);
	textVocabList->ChangeValue(s);
}



