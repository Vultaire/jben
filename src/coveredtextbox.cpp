/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: coveredtextbox.cpp

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

#include "coveredtextbox.h"

BEGIN_EVENT_TABLE(CoveredTextBox, wxTextCtrl)
	/* */
	EVT_KEY_UP(CoveredTextBox::OnKeyUp)
	EVT_LEFT_UP(CoveredTextBox::OnMouseLeftUp)
END_EVENT_TABLE()

CoveredTextBox::CoveredTextBox(wxWindow *parent, int id, const wxString& coverString, const wxString& hiddenString, bool isCovered)
  : wxTextCtrl(parent, id, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY) {
	normalbg = this->GetBackgroundColour();
	hidden = hiddenString;
	cover = coverString;
	Cover(isCovered);
}

void CoveredTextBox::UpdateValue() {
	if(covered) {
		this->SetValue(cover);
	} else {
		this->SetValue(hidden);
	}
	this->SetSelection(0,0);
}

void CoveredTextBox::Uncover() {
	covered=false;
	this->SetBackgroundColour(normalbg);
	UpdateValue();
}

void CoveredTextBox::Cover() {
	covered=true;
	this->SetBackgroundColour(*wxLIGHT_GREY);
	UpdateValue();
}

void CoveredTextBox::Cover(bool state) {
	if(state) Cover();
	else Uncover();
}

void CoveredTextBox::OnKeyUp(wxKeyEvent& event) {
	if(covered && event.GetKeyCode()==WXK_SPACE) Uncover();
}
void CoveredTextBox::OnMouseLeftUp(wxMouseEvent& event) {
	if(covered) Uncover();
	event.Skip();
}

void CoveredTextBox::SetHiddenStr(const wxString& s) {hidden=s;}
void CoveredTextBox::SetCoverStr(const wxString& s) {cover=s;}
