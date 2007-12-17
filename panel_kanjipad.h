/*
Project: wxKanjiPad
Author:  Paul Goins
Website: http://www.vultaire.net/software/wxkanjipad/files/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_kanjipad.h

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

#ifndef panel_kanjipad_h
#define panel_kanjipad_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/process.h"
#include <list>
using namespace std;

#define KANJIPAD_MAX_KANJI 5

class PanelKanjiPad: public wxPanel {
public:
	PanelKanjiPad(wxWindow *owner, wxWindowID id=wxID_ANY,
				  const wxPoint& pos=wxDefaultPosition,
				  const wxSize& size=wxDefaultSize,
				  long style=wxBORDER_SIMPLE,
				  const wxString& name = _T("panelkanjipad"));
	/* Public methods */
	void Clear();
	list< list<wxPoint> > const* GetStrokes();
	bool Search();
	/* Events */
	void OnMouseDown(wxMouseEvent& ev);
	void OnMouseUp(wxMouseEvent& ev);
	void OnLeaveWindow(wxMouseEvent& ev);
	void OnMouseMove(wxMouseEvent& ev);
	void OnMouseRightDown(wxMouseEvent& ev);
	void OnPaint(wxPaintEvent& ev);
	void AfterEngineCall(wxProcessEvent& ev);
	/* Public Vars */
	wxChar results[KANJIPAD_MAX_KANJI];

private:
	list<wxPoint> currentStroke;
	list< list<wxPoint> > strokes;
	bool isDrawing;
	/* Related to running kpengine process */
	wxProcess *kpengine;
	wxInputStream *kpStdout, *kpStderr;
	wxOutputStream *kpStdin;
	long kpPid;
	/* wxWidgets standard stuff */
	DECLARE_EVENT_TABLE()
};

#endif
