/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_worddict.h

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

#ifndef panel_worddict_h
#define panel_worddict_h

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

class PanelWordDict: public RedrawablePanel {
public:
	PanelWordDict(wxWindow *owner);
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
	/*wxString currentVocab;*/
	wxString currentSearchString;

	void UpdateHtmlOutput();
	DECLARE_EVENT_TABLE()
};

#endif
