/*
Project: wxKanjiPad
Author:  Paul Goins
Website: http://www.vultaire.net/software/wxkanjipad/files/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: frame_kanjipad.h

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef frame_kanjipad_h
#define frame_kanjipad_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/listctrl.h"
#include "panel_kanjipad.h"

class FrameKanjiPad: public wxFrame {
public:
	FrameKanjiPad(wxWindow *owner=(wxWindow *)NULL,
				  wxWindowID id = -1,
				  const wxString& title = _T("wxKanjiPad"),
				  const wxPoint& pos = wxDefaultPosition,
				  const wxSize& size = wxSize(300,350));
	void OnClose(wxCloseEvent& ev);
	void OnButtonSearch(wxCommandEvent& ev);
	void OnButtonClear(wxCommandEvent& ev);
	void OnKanjiSelected(wxListEvent& ev);
private:
	PanelKanjiPad *pnlKanjiPad;
	wxListCtrl *lcKanjiSelector;
	wxButton *btnClear, *btnSearch;
	DECLARE_EVENT_TABLE()
};

#endif
