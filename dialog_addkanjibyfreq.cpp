/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_addkanjibyfreq.cpp

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

#include "dialog_addkanjibyfreq.h"

enum {
	ID_buttonOK=wxID_OK,
	ID_buttonCancel=wxID_CANCEL,
	ID_spinLow=1,
	ID_spinHigh
};

BEGIN_EVENT_TABLE(DialogAddKanjiByFreq, wxDialog)
	EVT_BUTTON(ID_buttonOK, DialogAddKanjiByFreq::OnOK)
	EVT_BUTTON(ID_buttonCancel, DialogAddKanjiByFreq::OnCancel)
	EVT_KEY_DOWN(DialogAddKanjiByFreq::OnKeyDown)
	EVT_SPINCTRL(ID_spinLow, DialogAddKanjiByFreq::OnLowValChange)
	EVT_SPINCTRL(ID_spinHigh, DialogAddKanjiByFreq::OnHighValChange)
END_EVENT_TABLE()

DialogAddKanjiByFreq::DialogAddKanjiByFreq(wxWindow *parent)
  : wxDialog(parent, wxID_ANY, wxString(_T("Add Kanji By Grade"))) {
	spinLowFreq = new wxSpinCtrl(this, ID_spinLow, wxEmptyString,
		 wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS,	1, 2501, 1);
	spinHighFreq = new wxSpinCtrl(this, ID_spinHigh, wxEmptyString,
		 wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS,	1, 2501, 2501);

	btnOK = new wxButton(this, ID_buttonOK, _T("OK"));
	btnCancel = new wxButton(this, ID_buttonCancel, _T("Cancel"));
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(spinLowFreq, 0, wxALL, 10);
	mainSizer->Add(spinHighFreq, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(btnOK, 0, wxRIGHT, 10);
	buttonSizer->Add(btnCancel, 0);
	mainSizer->Add(buttonSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
	this->SetSizerAndFit(mainSizer);
	spinLowFreq->SetFocus();
}

int DialogAddKanjiByFreq::GetLowFreq() {
	return spinLowFreq->GetValue();
}

int DialogAddKanjiByFreq::GetHighFreq() {
	return spinHighFreq->GetValue();
}

void DialogAddKanjiByFreq::OKProc() {
	int l = GetLowFreq();
	int h = GetHighFreq();
	if(h<l)
		wxMessageBox(_T("Your high frequency rank cannot be less than your low frequency rank."), _T("Bad frequency range"), wxOK | wxICON_WARNING, this);
	else
		EndModal(ID_buttonOK);
}

void DialogAddKanjiByFreq::CancelProc() {
	EndModal(ID_buttonCancel);
}

void DialogAddKanjiByFreq::OnOK(wxCommandEvent& ev) {OKProc();}
void DialogAddKanjiByFreq::OnCancel(wxCommandEvent& ev) {CancelProc();}

void DialogAddKanjiByFreq::OnKeyDown(wxKeyEvent& ev) {
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

void DialogAddKanjiByFreq::OnLowValChange(wxSpinEvent& ev) {
	int low, high;
	low = spinLowFreq->GetValue();
	high = spinHighFreq->GetValue();
	if(low>high)
		spinHighFreq->SetValue(low);
}

void DialogAddKanjiByFreq::OnHighValChange(wxSpinEvent& ev) {
	int low, high;
	low = spinLowFreq->GetValue();
	high = spinHighFreq->GetValue();
	if(high<low)
		spinLowFreq->SetValue(high);
}
