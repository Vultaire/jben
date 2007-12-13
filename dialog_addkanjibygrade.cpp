/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_addkanjibygrade.cpp

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

#include "dialog_addkanjibygrade.h"

enum {
	ID_buttonOK=wxID_OK,
	ID_buttonCancel=wxID_CANCEL,
	ID_comboLow=1,
	ID_comboHigh
};

BEGIN_EVENT_TABLE(DialogAddKanjiByGrade, wxDialog)
	EVT_BUTTON(ID_buttonOK, DialogAddKanjiByGrade::OnOK)
	EVT_BUTTON(ID_buttonCancel, DialogAddKanjiByGrade::OnCancel)
	EVT_KEY_DOWN(DialogAddKanjiByGrade::OnKeyDown)
	EVT_COMBOBOX(ID_comboLow, DialogAddKanjiByGrade::OnLowValChange)
	EVT_COMBOBOX(ID_comboHigh, DialogAddKanjiByGrade::OnHighValChange)
END_EVENT_TABLE()

wxString comboStrings[] = {
	_T("Jouyou Kanji Grade 1"),
	_T("Jouyou Kanji Grade 2"),
	_T("Jouyou Kanji Grade 3"),
	_T("Jouyou Kanji Grade 4"),
	_T("Jouyou Kanji Grade 5"),
	_T("Jouyou Kanji Grade 6"),
	_T("Jouyou Kanji, General Usage"),
	_T("Jinmeiyou Kanji (for names)"),
	_T("Non-Jouyou/Non-Jinmeiyou Kanji")
};
int comboStringCount = 9;

DialogAddKanjiByGrade::DialogAddKanjiByGrade(wxWindow *parent)
  : wxDialog(parent, wxID_ANY, wxString(_T("Add Kanji By Grade"))) {
	comboLowGrade = new wxComboBox(this, ID_comboLow, comboStrings[0], wxDefaultPosition, wxDefaultSize, 9, comboStrings, wxCB_DROPDOWN | wxCB_READONLY);
	comboHighGrade = new wxComboBox(this, ID_comboHigh, comboStrings[0], wxDefaultPosition, wxDefaultSize, 9, comboStrings, wxCB_DROPDOWN | wxCB_READONLY);

	btnOK = new wxButton(this, ID_buttonOK, _T("OK"));
	btnCancel = new wxButton(this, ID_buttonCancel, _T("Cancel"));
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(comboLowGrade, 0, wxALL, 10);
	mainSizer->Add(comboHighGrade, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(btnOK, 0, wxRIGHT, 10);
	buttonSizer->Add(btnCancel, 0);
	mainSizer->Add(buttonSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
	this->SetSizerAndFit(mainSizer);
	comboLowGrade->SetFocus();
}

int ComboBoxToKanjidicGrade(wxComboBox *c) {
	int i;
	for(i=0;i<comboStringCount;i++)
		if(c->GetValue()==comboStrings[i]) break;
	if(i<6) return i+1;	/* G1-G6 in KANJIDIC */
	if(i==6) return 8;  /* G8 (Jouyou Jr. High) in KANJIDIC */
	if(i==7) return 9;  /* G9 (Jinmeiyou) in KANJIDIC */
	return 0;           /* No grade listed in KANJIDIC */
}

int DialogAddKanjiByGrade::GetLowGrade() {
	return ComboBoxToKanjidicGrade(comboLowGrade);
}

int DialogAddKanjiByGrade::GetHighGrade() {
	return ComboBoxToKanjidicGrade(comboHighGrade);
}

#if 0
void DialogAddKanjiByGrade::OnClose(wxCloseEvent& ev) {
	printf("OnClose event entered...\n");
	if(ev.CanVeto()) {
		printf("Event CAN be vetoed...\n");
		int l = GetLowGrade();
		int h = GetHighGrade();
		if(h<l && h!=0) { /* 0 is a special code for non-graded kanji and is treated as the highest grade level here. */
			wxMessageBox(_T("Your high grade level cannot be less than your low grade level."), _T("Bad grade range"), wxOK | wxICON_WARNING, this);
			ev.Veto(true);
		}
	}
}
#endif

void DialogAddKanjiByGrade::OKProc() {
	int l = GetLowGrade();
	int h = GetHighGrade();
	/* 0 is a special code for non-graded kanji and is treated
	   as the highest grade level here. */
	if((h<l && h!=0) || (l==0 && h!=0))
		wxMessageBox(_T("Your high grade level cannot be less than your low grade level."), _T("Bad grade range"), wxOK | wxICON_WARNING, this);
	else
		EndModal(ID_buttonOK);
}

void DialogAddKanjiByGrade::CancelProc() {
	EndModal(ID_buttonCancel);
}

void DialogAddKanjiByGrade::OnOK(wxCommandEvent& ev) {OKProc();}
void DialogAddKanjiByGrade::OnCancel(wxCommandEvent& ev) {CancelProc();}

void DialogAddKanjiByGrade::OnKeyDown(wxKeyEvent& ev) {
	int i = ev.GetKeyCode();
	switch(i) {
	case WXK_ESCAPE:
		CancelProc();
		break;
	case WXK_RETURN:
		OKProc();
		break;
	default:
		ev.Skip();
	}
}

void DialogAddKanjiByGrade::OnLowValChange(wxCommandEvent& ev) {
	int low, high;
	low = GetLowGrade();
	high = GetHighGrade();
	if(high!=0 && (low>high || low==0))
		comboHighGrade->SetValue(comboLowGrade->GetValue());
}

void DialogAddKanjiByGrade::OnHighValChange(wxCommandEvent& ev) {
	int low, high;
	low = GetLowGrade();
	high = GetHighGrade();
	if(high!=0 && (high<low || low==0))
		comboLowGrade->SetValue(comboHighGrade->GetValue());
}
