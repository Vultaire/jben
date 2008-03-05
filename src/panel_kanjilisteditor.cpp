/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_kanjilisteditor.cpp

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

#include "panel_kanjilisteditor.h"
#include "jben.h"
#include "listmanager.h"

enum {
	ID_btnRevert=1,
	ID_btnCommit,
	ID_textKanjiList
};

BEGIN_EVENT_TABLE(PanelKanjiListEditor, wxPanel)
	EVT_BUTTON(ID_btnRevert, PanelKanjiListEditor::OnRevert)
	EVT_BUTTON(ID_btnCommit, PanelKanjiListEditor::OnCommit)
	EVT_TEXT(ID_textKanjiList, PanelKanjiListEditor::OnTextChanged)
END_EVENT_TABLE()

PanelKanjiListEditor::PanelKanjiListEditor(wxWindow *owner) : RedrawablePanel(owner) {
	textKanjiList = new wxTextCtrl(this, ID_textKanjiList, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxButton *btnRevert = new wxButton(this, ID_btnRevert, _T("Revert"));
	wxButton *btnCommit = new wxButton(this, ID_btnCommit, _T("Commit"));

	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	buttonSizer->AddStretchSpacer(1);
	buttonSizer->Add(btnRevert);
	buttonSizer->AddStretchSpacer(1);
	/*buttonSizer->AddStretchSpacer(1);*/
	buttonSizer->Add(btnCommit);
	buttonSizer->AddStretchSpacer(1);
	panelSizer->Add(textKanjiList, 1, wxEXPAND | wxALL, 10);
	panelSizer->Add(buttonSizer, 0, wxEXPAND | wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10);

	changeDetected = false;
	this->SetSizerAndFit(panelSizer);
}

void PanelKanjiListEditor::Commit() {
	ListManager* lm = ListManager::Get();
	changeDetected = false;
	lm->KList()->Clear();
	int result = lm->KList()->AddFromString(
		textKanjiList->GetValue().c_str());

	jben->gui->Redraw();

	Redraw();	/* Might be redundant now with the above Redraw() call... */
	wxMessageBox(wxString::Format(_T("Changes Saved.\nTotal kanji in list: %d"), result),
				_T("Kanji List Editor"), wxOK | wxICON_INFORMATION, this);
}

void PanelKanjiListEditor::Revert() {
	changeDetected = false;
	Redraw();
}

void PanelKanjiListEditor::OnRevert(wxCommandEvent& ev) {Revert();}
void PanelKanjiListEditor::OnCommit(wxCommandEvent& ev) {Commit();}

void PanelKanjiListEditor::OnTextChanged(wxCommandEvent& ev) {
	changeDetected = true;
}

bool PanelKanjiListEditor::ChangeDetected() {return changeDetected;}

void PanelKanjiListEditor::Redraw() {
	ListManager* lm = ListManager::Get();
	textKanjiList->ChangeValue(lm->KList()->ToString(20));
}
