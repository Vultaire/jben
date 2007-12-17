/*
Project: wxKanjiPad
Author:  Paul Goins
Website: http://www.vultaire.net/software/wxkanjipad/files/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: panel_kanjipad.cpp

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

#include "panel_kanjipad.h"

BEGIN_EVENT_TABLE(PanelKanjiPad, wxPanel)
	EVT_LEFT_DOWN(PanelKanjiPad::OnMouseDown)
	EVT_LEFT_UP(PanelKanjiPad::OnMouseUp)
	EVT_LEAVE_WINDOW(PanelKanjiPad::OnLeaveWindow)
	EVT_MOTION(PanelKanjiPad::OnMouseMove)
	EVT_RIGHT_DOWN(PanelKanjiPad::OnMouseRightDown)
	EVT_PAINT(PanelKanjiPad::OnPaint)
	EVT_END_PROCESS(wxID_ANY, PanelKanjiPad::AfterEngineCall)
END_EVENT_TABLE()

PanelKanjiPad::PanelKanjiPad(wxWindow *owner,
				   wxWindowID id,
				   const wxPoint& pos,
				   const wxSize& size,
				   long style,
				   const wxString& name) : wxPanel(owner, id, pos,
												   size, style, name) {
	isDrawing=false;
	kpengine=NULL;
	kpStdin=NULL;
	kpStdout=kpStderr=NULL;
	kpPid=0;
	for(int i=0;i<KANJIPAD_MAX_KANJI;i++) results[i]=_T('\0');
}

void PanelKanjiPad::OnMouseDown(wxMouseEvent& ev) {
	currentStroke.push_back(ev.GetPosition());
#ifdef DEBUG
	printf("MouseDown: [%d/%d]\n", currentStroke.back().x, currentStroke.back().y);
#endif
	isDrawing=true;
}

void PanelKanjiPad::OnMouseUp(wxMouseEvent& ev) {
	if(isDrawing) {
		isDrawing=false;
#ifdef DEBUG
		printf("MouseUp: [%d/%d]\n", currentStroke.back().x, currentStroke.back().y);
#endif
		strokes.push_back(currentStroke);
		currentStroke.clear();
	}
}

void PanelKanjiPad::OnLeaveWindow(wxMouseEvent& ev) {
	if(isDrawing) OnMouseUp(ev);
}

void PanelKanjiPad::OnMouseMove(wxMouseEvent& ev) {
	if(isDrawing) {
		wxPoint pCurrent = ev.GetPosition();
		wxPoint pLast = currentStroke.back();
		wxClientDC dc(this);

		/* Do drawing */
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.DrawLine(pLast.x, pLast.y, pCurrent.x, pCurrent.y);
		dc.SetBrush(wxNullBrush);
		
		/* Append point to stroke */
		currentStroke.push_back(pCurrent);
#ifdef DEBUG
		printf("MouseMove: [%d/%d]\n", currentStroke.back().x, currentStroke.back().y);
#endif
	}
}

void PanelKanjiPad::Clear() {
	currentStroke.clear();
	strokes.clear();
}

void PanelKanjiPad::OnPaint(wxPaintEvent& ev) {
	wxPaintDC dc(this);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.SetBrush(*wxBLACK_BRUSH);

	wxPoint *pList;
	int pCount, i;

	dc.Clear();
	/* Draw all strokes */
	for(list< list<wxPoint> >::iterator li = strokes.begin();
		li!=strokes.end(); li++) {
		pCount = li->size();
		pList = new wxPoint[pCount];
		i=0;
		for(list<wxPoint>::iterator pi = li->begin();
			pi != li->end(); pi++) {
			pList[i] = *pi;
			i++;
		}
		dc.DrawLines(pCount, pList);
		delete[] pList;
	}

	dc.SetBrush(wxNullBrush);
}

void PanelKanjiPad::OnMouseRightDown(wxMouseEvent& ev) {
	if(isDrawing) {
		wxPoint p = ev.GetPosition();
		currentStroke.clear();
		currentStroke.push_back(p);
	} else {
		/* For some reason, this call is slow! */
		if(strokes.size()>0) strokes.pop_back();
	}
	this->Refresh();
}

list< list<wxPoint> > const* PanelKanjiPad::GetStrokes() {return &strokes;}

bool PanelKanjiPad::Search() {
	/* Create string to send to back end */
	/* kpengine takes alternating x y args for points via stdin.
	   '\n' breaks between the strokes. */
	char buffer[30];
	string points("");
	size_t charsWritten;

	if(!kpengine) {
		if(strokes.size()<1) return false;
		for(list< list<wxPoint> >::const_iterator li=strokes.begin();
			li!=strokes.end(); li++) {
			for(list<wxPoint>::const_iterator pi=li->begin();
				pi!=li->end(); pi++) {
				charsWritten = sprintf(buffer, "%d %d ", pi->x, pi->y);
				buffer[charsWritten]='\0';
				points.append(buffer);
			}
			points.append("\n");
		}
		points.append("\n");

		/* Connect to back end */
		kpengine = new wxProcess(this);
		kpengine->Redirect();
#ifdef __MSWINDOWS__
		wxString command = _T(".\\kpengine");
#else
		wxString command = _T("./kpengine");
#endif
		kpPid = wxExecute(command, wxEXEC_ASYNC, kpengine);

		/* Grab our streams */
		kpStdin = kpengine->GetOutputStream();
		kpStdout = kpengine->GetInputStream();
		kpStderr = kpengine->GetErrorStream();

		if(!kpPid) {
			wxMessageBox(_T("ERROR: kpengine could not start!"),
						 _T("Error!"), wxOK | wxICON_ERROR, this);
		} else if(!kpStdin || !kpStdout || !kpStderr) {
			wxMessageBox(_T("ERROR: Could not capture streams from kpengine!"),
						 _T("Error!"), wxOK | wxICON_ERROR, this);
			wxKillError killres = wxProcess::Kill(kpPid, wxSIGTERM);
			if(killres!=wxKILL_OK)
				wxMessageBox(wxString::Format(
							 _T("ERROR: Sending SIGTERM to kpengine returned "
								"code %d!\n"), killres),
							 _T("Error!"), wxOK | wxICON_ERROR, this);
			kpPid = 0;
		}
		/* If either of the above errors occurred, then bail. */
		if(kpPid==0) {
			delete kpengine;
			kpengine = NULL;
			return false;
		}
		

		/* Write all data out to kpengine's stdin */
		size_t len = points.length();
		charsWritten = 0;
		while(charsWritten<len) {
			kpStdin->Write((const void *)(points.c_str() + charsWritten),
						   points.length());
			charsWritten+=kpStdin->LastWrite();
		}
		kpStdin->Close();

		/* Wait until the child process terminates */
		/* However, don't wait forever!  After 3 seconds, force it to close! */
		wxLongLong stime = wxGetLocalTime();
		while(kpengine && (wxGetLocalTime() - stime < 3000))
			wxTheApp->Yield();

		return (results[0] != _T('\0'));
	} return false;
}

void PanelKanjiPad::AfterEngineCall(wxProcessEvent& ev) {
	if(ev.GetPid()==kpPid) {
		if(kpengine) {
			char buffer[81];
			size_t index, len;

			memset((void *)buffer, 0, 81);

			for(int i=0;i<KANJIPAD_MAX_KANJI;i++) results[i]=_T('\0');
			if(kpStdout->CanRead()) {
				kpStdout->Read(buffer, 80);
			}
			if(buffer[0]=='K') {
				index = 1;
				len = strlen(buffer);
				for(int i=0;i<KANJIPAD_MAX_KANJI;i++) {
					if(index<strlen(buffer)) {
						results[i] = atoi(buffer+index);
						index = strchr(buffer+index, ' ') - buffer + 1;
					}
				}
			}

			buffer[0] = '\0';
			if(kpStderr->CanRead()) {
				kpStderr->Read(buffer, 80);
			}

			if(strlen(buffer)>0) {
				string errstr("Errors: [");
				errstr.append(buffer);
				while(kpStderr->CanRead()) {
					kpStderr->Read(buffer, 80);
					errstr.append(buffer);
				}
				errstr.append("]\n");
				wxMessageBox(wxString(errstr.c_str(), wxConvUTF8),
							 _T("Error on kpengine call"),
							 wxOK | wxICON_ERROR, this);
			}
		
			delete kpengine;
			kpengine = NULL;
			kpPid=0;
		}
	}
}
