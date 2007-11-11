/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_kanjidict.h

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

#ifndef panel_kanjidict_h
#define panel_kanjidict_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "redrawablepanel.h"
#include "wx/html/htmlwin.h"
#include "wx/stattext.h"

class PanelKanjiDict: public RedrawablePanel {
public:
	PanelKanjiDict(wxWindow *owner);
	void OnSearch(wxCommandEvent& event);
	void OnPrevious(wxCommandEvent& event);
	void OnNext(wxCommandEvent& event);
	void OnRandom(wxCommandEvent& event);
	void OnIndexUpdate(wxCommandEvent& event);
	void Redraw();
	void SetSearchString(const wxString& searchString);
private:
	wxTextCtrl *textSearch;
	wxHtmlWindow *htmlOutput;
	wxTextCtrl *textIndex;
	wxStaticText *lblIndex;

	int currentIndex;
	wxChar currentKanji;
	wxString currentSearchString;

	void UpdateHtmlOutput();
	DECLARE_EVENT_TABLE()
};

#endif
