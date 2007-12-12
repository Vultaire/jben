/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: maingui.h

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

#ifndef gui_h
#define gui_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "dialog_addkanjibygrade.h"
#include "dialog_addkanjibyfreq.h"
#include "panel_kanjidict.h"
#include "panel_worddict.h"
#include "panel_kanjidrill.h"
#include "panel_kanjilisteditor.h"
#include "panel_vocablisteditor.h"
#include "panel_config.h"
#include "wx/notebook.h"

/* This is defined simply to help external code enable/disable GUI menus
   when necessary. */
enum {
	GUI_menuFile=0,
	GUI_menuKanji,
	GUI_menuHelp
};

class MainGUI: public wxFrame {
public:
	MainGUI();

	void OnClose(wxCloseEvent& event);

	void OnFileQuit(wxCommandEvent& event);

	void OnKanjiAddFromFile(wxCommandEvent& event);
	void OnKanjiAddByGrade(wxCommandEvent& event);
	void OnKanjiAddByFreq(wxCommandEvent& event);
	void OnKanjiSaveToFile(wxCommandEvent& event);
	void OnKanjiClearList(wxCommandEvent& event);
	void OnKanjiSortByGrade(wxCommandEvent& event);
	void OnKanjiSortByFreq(wxCommandEvent& event);
#ifdef DEBUG
	void OnKanjiDumpList(wxCommandEvent& event);
#endif
	void OnKanjiSearchKanjiPad(wxCommandEvent& event);
	
	void OnHelpAbout(wxCommandEvent& event);
	void OnHelpLicense(wxCommandEvent& event);

	void OnTabChanging(wxNotebookEvent& event);
	void OnMajorTabChanged(wxNotebookEvent& event);
	void OnMinorTabChanged(wxNotebookEvent& event);

	void Redraw();
private:
	bool TabChangeHandler(wxNotebookPage *page);

	wxMenu *kanjiMenu;
	wxNotebook *tabsMain, *tabsKanji, *tabsWords, *tabsConfig;
	wxNotebookPage *tabMajor, *tabMinor;
	PanelKanjiDict *panelKanjiDict;
	PanelWordDict *panelWordDict;
	PanelKanjiDrill *panelKanjiDrill;
	PanelKanjiListEditor *panelKanjiListEditor;
	PanelVocabListEditor *panelVocabListEditor;
	PanelConfig *panelConfig;
	DialogAddKanjiByGrade *dialogAddKanjiByGrade;
	DialogAddKanjiByFreq *dialogAddKanjiByFreq;
	DECLARE_EVENT_TABLE()
};

#endif
