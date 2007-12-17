/*
Project: wxKanjiPad
Author:  Paul Goins
Website: http://www.vultaire.net/software/wxkanjipad/files/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: frame_kanjipad.cpp

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

#include "frame_kanjipad.h"

/* Clipboard Workaround */
#ifdef __WXGTK__
#include <gtk/gtk.h>
#else
#include <wx/clipbrd.h>
#endif

enum {
	ID_btnSearch=1,
	ID_btnClear,
	ID_lcKanjiSelector
};

BEGIN_EVENT_TABLE(FrameKanjiPad, wxFrame)
	EVT_CLOSE(FrameKanjiPad::OnClose)
	EVT_BUTTON(ID_btnSearch, FrameKanjiPad::OnButtonSearch)
	EVT_BUTTON(ID_btnClear, FrameKanjiPad::OnButtonClear)
	EVT_LIST_ITEM_ACTIVATED(ID_lcKanjiSelector, FrameKanjiPad::OnKanjiSelected)
END_EVENT_TABLE()

FrameKanjiPad
::FrameKanjiPad(wxWindow *owner, wxWindowID id, const wxString& title,
				const wxPoint& pos, const wxSize& size)
: wxFrame(owner, id, title, pos, size) {
	/* Create all the simple stuff */
	pnlKanjiPad = new PanelKanjiPad(this, -1,
									wxDefaultPosition, wxDefaultSize,
									wxBORDER_SUNKEN);
	btnSearch = new wxButton(this, ID_btnSearch, _T("&Search"));
	btnClear = new wxButton(this, ID_btnClear, _T("&Clear"));

	/* Create the kanji selector */
	/* Our kanji selector should use a larger font than standard.
	   Let's do a hard-coded 32-pt font for our purposes. */
	lcKanjiSelector = new wxListCtrl(this, ID_lcKanjiSelector,
									 wxDefaultPosition,
									 wxSize(-1, 25),
									 wxLC_LIST | wxLC_SINGLE_SEL
									 | wxHSCROLL | wxBORDER_SUNKEN);
	wxFont fntKanjiSelector(this->GetFont());
	fntKanjiSelector.SetPointSize(32);
	lcKanjiSelector->SetFont(fntKanjiSelector);
	/* Resize the kanji selector to fit the font appropriately */
	/* NOTE: to get the proper size under WIN32, we must explicitly pass the font into
	   the control's wxClientDC, even though the font for the object was already set
          before obtaining the DC. */
	wxClientDC cdc(lcKanjiSelector);
	cdc.SetFont(fntKanjiSelector);
	wxSize szFnt = cdc.GetTextExtent(_T("漢"));
	lcKanjiSelector->SetColumnWidth(-1, szFnt.GetWidth()+6); /* hard-coded padding */
	szFnt.SetWidth(-1);
	szFnt.SetHeight(szFnt.GetHeight()+12); /* Hard-coded padding */
	lcKanjiSelector->SetSize(szFnt);
	lcKanjiSelector->SetMinSize(szFnt);

	/* Arrange controls and add to frame */
	wxBoxSizer *vertSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *horizSizer = new wxBoxSizer(wxHORIZONTAL);
	vertSizer->Add(pnlKanjiPad, 1, wxEXPAND);
	vertSizer->Add(lcKanjiSelector, 0, wxEXPAND);
	horizSizer->AddStretchSpacer(1);
	horizSizer->Add(btnSearch, 0, wxALIGN_CENTER_HORIZONTAL);
	horizSizer->AddStretchSpacer(1);
	horizSizer->Add(btnClear, 0, wxALIGN_CENTER_HORIZONTAL);
	horizSizer->AddStretchSpacer(1);
	vertSizer->Add(horizSizer, 0, wxEXPAND);
	SetSizer(vertSizer);
}

void FrameKanjiPad::OnClose(wxCloseEvent& ev) {
	this->Destroy();
}

void FrameKanjiPad::OnButtonSearch(wxCommandEvent& ev) {
	if(pnlKanjiPad->Search()) {
		lcKanjiSelector->ClearAll();
		for(int i=0;i<KANJIPAD_MAX_KANJI;i++) {
			if(pnlKanjiPad->results[i]==_T('\0')) break;
			lcKanjiSelector->InsertItem(i, wxString(pnlKanjiPad->results[i]));
		}
	}
}

void FrameKanjiPad::OnButtonClear(wxCommandEvent& ev) {
	pnlKanjiPad->Clear();
	lcKanjiSelector->ClearAll();
	Refresh();
	/*pnlKanjiPad->Refresh();*/
}

void FrameKanjiPad::OnKanjiSelected(wxListEvent& ev) {
	int i = (int) lcKanjiSelector->
		GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	/* Check bounds */
	if(i < 0 || i > KANJIPAD_MAX_KANJI-1) {
		wxMessageBox(wxString::Format(
						 _T("Error: selected item is reported as %d."), i),
					 _T("Error"), wxOK | wxICON_ERROR, this);
		return;
	}	

#ifdef __WXGTK__
	/* Copy to clipboard - GTK Style*/
	/* Convert from wide char to multibyte char in native encoding */
	wxString ws(pnlKanjiPad->results[i]);
	int bufSize = sizeof(wxChar) * (ws.length()+1);
	gchar *buffer = new gchar[bufSize];
	memset(buffer, '\0', bufSize*sizeof(gchar));
	wxConvUTF8.WC2MB(buffer, ws.c_str(), bufSize);

	/* Copy to clipboard */
	GtkClipboard *gClip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(gClip, buffer, strlen(buffer));
	delete[] buffer;
	/* The clipboard data -seems- to be persistant after termination of the
	   program, but in the case it is not, try fiddling with the below two
	   calls.  I couldn't get the first one to work right. */
#if 0
	gtk_clipboard_set_can_store(gClip, NULL, 1);
	gtk_clipboard_store(gClip);
#endif
#else /* Copy to clipboard - wxWidgets Style */
	/* Copy to clipboard */
	if(wxTheClipboard->Open()) {
		wxTheClipboard->Clear();
		wxTheClipboard->SetData(
			new wxTextDataObject(wxString(pnlKanjiPad->results[i])));
		if(!wxTheClipboard->Flush()) {
#if 0
			/* Flush fails often on non-Windows platforms, so for now I'm
			   just gonna disable this message box. */
			wxMessageBox(_T("Error: Could not flush data to clipboard."),
						 _T("Error"), wxOK | wxICON_ERROR, this);
#endif
		}
		wxTheClipboard->Close();
	} else {
		wxMessageBox(_T("Error: Could not open clipboard."),
					 _T("Error"), wxOK | wxICON_ERROR, this);
	}
#endif
}
