/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_vocablisteditor.h

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

#ifndef panel_vocablisteditor_h
#define panel_vocablisteditor_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "redrawablepanel.h"

class PanelVocabListEditor: public RedrawablePanel {
public:
	PanelVocabListEditor(wxWindow *owner);
	void Commit();
	void Revert();
	void OnRevert(wxCommandEvent& ev);
	void OnCommit(wxCommandEvent& ev);
	void OnTextChanged(wxCommandEvent& ev);
	void Redraw();
	bool ChangeDetected();
private:
	bool changeDetected;
	wxTextCtrl *textVocabList;
	DECLARE_EVENT_TABLE()
};

#endif
