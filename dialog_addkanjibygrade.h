/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dialog_addkanjibygrade.h

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

#ifndef dialog_addkanjibygrade_h
#define dialog_addkanjibygrade_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx/combobox.h"

class DialogAddKanjiByGrade : public wxDialog {
public:
	DialogAddKanjiByGrade(wxWindow *parent);
	void OnOK(wxCommandEvent& ev);
	void OnCancel(wxCommandEvent& ev);
	void OnKeyDown(wxKeyEvent& ev);
	void OnLowValChange(wxCommandEvent& ev);
	void OnHighValChange(wxCommandEvent& ev);
	int GetLowGrade();
	int GetHighGrade();
	/*void OnClose(wxCloseEvent& ev);*/
private:
	wxComboBox *comboLowGrade, *comboHighGrade;
	wxButton *btnOK, *btnCancel;
	void OKProc();
	void CancelProc();
	DECLARE_EVENT_TABLE()
};


#endif
