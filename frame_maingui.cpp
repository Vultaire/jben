/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: maingui.cpp

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

#include "frame_maingui.h"
#include "jben.h"
#include "version.h"
#include "wx/msgdlg.h"
#include "file_utils.h"
#include "wx/ffile.h"
#include "frame_kanjipad.h"
#include "encoding_convert.h"

#ifndef __WXMSW__
	#include "jben.xpm"
#endif

enum {
	ID_menuFileQuit=wxID_EXIT,
	ID_menuHelpAbout=wxID_ABOUT,

	ID_menuKanjiAddFromFile=1,
	ID_menuKanjiAddByGrade,
	ID_menuKanjiAddByFreq,
	ID_menuKanjiSaveToFile,
	ID_menuKanjiClearList,
#ifdef DEBUG
	ID_menuKanjiDumpList,
#endif
	ID_menuKanjiSortByGrade,
	ID_menuKanjiSortByFreq,
	ID_menuKanjiSearchKanjiPad,
	ID_menuHelpLicense,
	ID_tabsMain,
	ID_tabsKanji,
	ID_tabsWords,
	ID_tabsConfig
};

BEGIN_EVENT_TABLE(FrameMainGUI, wxFrame)
	EVT_CLOSE(FrameMainGUI::OnClose)

	EVT_MENU(ID_menuFileQuit, FrameMainGUI::OnFileQuit)

	EVT_MENU(ID_menuKanjiAddFromFile, FrameMainGUI::OnKanjiAddFromFile)
	EVT_MENU(ID_menuKanjiAddByGrade, FrameMainGUI::OnKanjiAddByGrade)
	EVT_MENU(ID_menuKanjiAddByFreq, FrameMainGUI::OnKanjiAddByFreq)
	EVT_MENU(ID_menuKanjiSaveToFile, FrameMainGUI::OnKanjiSaveToFile)
	EVT_MENU(ID_menuKanjiClearList, FrameMainGUI::OnKanjiClearList)
#ifdef DEBUG
	EVT_MENU(ID_menuKanjiDumpList, FrameMainGUI::OnKanjiDumpList)
#endif
	EVT_MENU(ID_menuKanjiSortByGrade, FrameMainGUI::OnKanjiSortByGrade)
	EVT_MENU(ID_menuKanjiSortByFreq, FrameMainGUI::OnKanjiSortByFreq)
	EVT_MENU(ID_menuKanjiSearchKanjiPad, FrameMainGUI::OnKanjiSearchKanjiPad)

	EVT_MENU(ID_menuHelpAbout, FrameMainGUI::OnHelpAbout)
	EVT_MENU(ID_menuHelpLicense, FrameMainGUI::OnHelpLicense)

	EVT_NOTEBOOK_PAGE_CHANGING(ID_tabsMain,   FrameMainGUI::OnTabChanging)
	EVT_NOTEBOOK_PAGE_CHANGED (ID_tabsMain,   FrameMainGUI::OnMajorTabChanged)
	EVT_NOTEBOOK_PAGE_CHANGING(ID_tabsKanji,  FrameMainGUI::OnTabChanging)
	EVT_NOTEBOOK_PAGE_CHANGED (ID_tabsKanji,  FrameMainGUI::OnMinorTabChanged)
	EVT_NOTEBOOK_PAGE_CHANGING(ID_tabsWords,  FrameMainGUI::OnTabChanging)
	EVT_NOTEBOOK_PAGE_CHANGED (ID_tabsWords,  FrameMainGUI::OnMinorTabChanged)
	EVT_NOTEBOOK_PAGE_CHANGING(ID_tabsConfig, FrameMainGUI::OnTabChanging)
	EVT_NOTEBOOK_PAGE_CHANGED (ID_tabsConfig, FrameMainGUI::OnMinorTabChanged)

END_EVENT_TABLE()

FrameMainGUI::FrameMainGUI() : wxFrame
((wxFrame *)NULL, -1,
 _T(PROGRAM_NAME " v" VERSION_STR " by " AUTHOR_NAME),
 wxDefaultPosition, wxSize(600,400)) {
	this->SetIcon(wxICON(iconJben));
	wxInitAllImageHandlers();	/* This is needed for PNG support - GIFs seem to
								   load fine without it. */

	/* Don't create these objects until/unless needed. */
	dialogAddKanjiByFreq = NULL;
	dialogAddKanjiByGrade = NULL;
	
	wxMenuBar *menuBar;
	wxMenu *menu, *subMenu;
	tabMinor = NULL;

	menuBar = new wxMenuBar;

	menu = new wxMenu;
	menu->Append(ID_menuFileQuit, _T("E&xit..."));
	menuBar->Append(menu, _T("&File"));

	/* Kanji Menu */
	kanjiMenu = new wxMenu;
	/* Submenu: Search for Kanji (probably a temporary menu, for now) */
	subMenu = new wxMenu;
	subMenu->Append(ID_menuKanjiSearchKanjiPad, _T("By &Handwriting"));
	kanjiMenu->AppendSubMenu(subMenu, _T("&Find Kanji"));
	/* Submenu: Add Kanji to List */
	subMenu = new wxMenu;
	subMenu->Append(ID_menuKanjiAddFromFile, _T("&From File..."));
	subMenu->Append(ID_menuKanjiAddByGrade, _T("By Jouyou &Grade Level..."));
	subMenu->Append(ID_menuKanjiAddByFreq,
					_T("By Newspaper F&requency Ranking..."));
	kanjiMenu->AppendSubMenu(subMenu, _T("&Add Kanji to List"));
	/* Submenu: Sort Kanji in List*/
	subMenu = new wxMenu;
	subMenu->Append(ID_menuKanjiSortByGrade, _T("By Jouyou &Grade Level"));
	subMenu->Append(ID_menuKanjiSortByFreq,
					_T("By Newspaper F&requency Ranking"));
	kanjiMenu->AppendSubMenu(subMenu, _T("S&ort Kanji in List"));
	/* Other Kanji Menu options */
	kanjiMenu->Append(ID_menuKanjiSaveToFile,
					  _T("&Save Kanji List to File..."));
	if(jben->kanjiList->Size()==0)
		kanjiMenu->Enable(ID_menuKanjiSaveToFile, false);
	kanjiMenu->AppendSeparator();
	kanjiMenu->Append(ID_menuKanjiClearList,
					  _T("&Clear Kanji List"));
	if(jben->kanjiList->Size()==0)
		kanjiMenu->Enable(ID_menuKanjiClearList, false);
#ifdef DEBUG
	kanjiMenu->Append(ID_menuKanjiDumpList, _T("&Dump Kanji List to Console"));
#endif
	menuBar->Append(kanjiMenu, _T("&Kanji"));

	menu = new wxMenu;
	menu->Append(ID_menuHelpAbout, _T("&About..."));
	menu->Append(ID_menuHelpLicense, _T("&License Information..."));
	menuBar->Append(menu, _T("&Help"));

	SetMenuBar(menuBar);

	tabsMain = new wxNotebook(this,
		ID_tabsMain,wxDefaultPosition,
		wxDefaultSize,wxCLIP_CHILDREN | wxNB_TOP);
#ifdef __WXMSW__
	tabsKanji = new wxNotebook(tabsMain,
		ID_tabsKanji,wxDefaultPosition,
		wxDefaultSize,wxCLIP_CHILDREN | wxNB_TOP);
	tabsWords = new wxNotebook(tabsMain,
		ID_tabsWords,wxDefaultPosition,
		wxDefaultSize,wxCLIP_CHILDREN | wxNB_TOP);
	tabsConfig = new wxNotebook(tabsMain,
		ID_tabsConfig,wxDefaultPosition,
		wxDefaultSize,wxCLIP_CHILDREN | wxNB_TOP);
#else
	tabsKanji = new wxNotebook(tabsMain,
		ID_tabsKanji,wxDefaultPosition,
		wxDefaultSize,wxCLIP_CHILDREN | wxNB_LEFT);
	tabsWords = new wxNotebook(tabsMain,
		ID_tabsWords,wxDefaultPosition,
		wxDefaultSize,wxCLIP_CHILDREN | wxNB_LEFT);
	tabsConfig = new wxNotebook(tabsMain,
		ID_tabsConfig,wxDefaultPosition,
		wxDefaultSize,wxCLIP_CHILDREN | wxNB_LEFT);
#endif
	tabsMain->AddPage(tabsKanji,_T("Kanji"),true);
	tabsMain->AddPage(tabsWords,_T("Words"),false);
	tabsMain->AddPage(tabsConfig,_T("Configuration"),false);
	tabMajor = (wxNotebookPage *) tabsKanji;

	panelKanjiDict = new PanelKanjiDict(tabsKanji);
	panelKanjiDrill = new PanelKanjiDrill(tabsKanji);
	panelKanjiListEditor = new PanelKanjiListEditor(tabsKanji);
	tabsKanji->AddPage(panelKanjiDict,_T("Dictionary"),true);
	tabsKanji->AddPage(panelKanjiDrill,_T("Practice"),false);
	tabsKanji->AddPage(panelKanjiListEditor,_T("Study List"),false);
	tabMinor = (wxNotebookPage *) panelKanjiDict;

	panelWordDict = new PanelWordDict(tabsWords);
	/* ADVANCED IDEA: Use radical database to create a multi-choice test option
	   for vocab and kanji.  That is, like on the JLPT for reading/writing
	   Kanji, use kanji which share one or more radicals or readings as
	   alternate options when possible. (Without this, panelVocabTest seems
	   somewhat pointless at this time.) */
	panelVocabListEditor = new PanelVocabListEditor(tabsWords);
	tabsWords->AddPage(panelWordDict,_T("Dictionary"),true);
	tabsWords->AddPage(panelVocabListEditor,_T("Study List"),false);

	panelConfig = new PanelConfig(tabsConfig);
	tabsConfig->AddPage(panelConfig,_T("Main Options"),true);
}

void FrameMainGUI::OnFileQuit(wxCommandEvent& event) {
	Close();
}

void FrameMainGUI::OnKanjiAddFromFile(wxCommandEvent& event) {
	/* NOTE: We may add the flag wxFD_CHANGE_DIR later, if we add in code
	   to save the full path for the jben.cfg file. */
	wxFileDialog *fd = new wxFileDialog(
		this, _T("Open File"), wxEmptyString, wxEmptyString, _T("*"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if(fd->ShowModal()==wxID_OK) {
		wxArrayString filenames;
		wstring file, allFiles;
		fd->GetPaths(filenames);
		for(unsigned int i=0;i<filenames.Count();i++) {
			if(ReadEncodedFile(
				   ConvertString<wchar_t,char>(filenames[i].c_str(),
											   wcType.c_str(), "UTF-8").c_str(),
				   file)==REF_SUCCESS)
				allFiles.append(file);
		}
		int result = jben->kanjiList->AddFromString(allFiles);
		this->Redraw();
		wxMessageBox(wxString::Format(
			_T("Added %d kanji to the list."), result),
			_T("Add Kanji From File"), wxOK | wxICON_INFORMATION, this);
	}
	fd->Destroy();
}

void FrameMainGUI::OnKanjiAddByGrade(wxCommandEvent& event) {
	if(!dialogAddKanjiByGrade)
		dialogAddKanjiByGrade = new DialogAddKanjiByGrade(this);
	int result = dialogAddKanjiByGrade->ShowModal();
	if(result==wxID_OK) {
		/*printf("Selected grades: %d to %d\n",
			dialogAddKanjiByGrade->GetLowGrade(),
			dialogAddKanjiByGrade->GetHighGrade()
		);*/
		result = jben->kanjiList->AddByGrade(
			dialogAddKanjiByGrade->GetLowGrade(),
			dialogAddKanjiByGrade->GetHighGrade());
		this->Redraw();
		wxMessageBox(wxString::Format(
			_T("Added %d kanji to the list."), result),
			_T("Add Kanji by Jouyou Grade"), wxOK | wxICON_INFORMATION, this);
	} else if(result==wxID_CANCEL) {
		/*printf("Cancel event occurred within dialog!\n");*/
	}
#ifdef DEBUG
	else printf("Unknown key (%08X) pressed within dialog!\n", result);
#endif
}

void FrameMainGUI::OnKanjiAddByFreq(wxCommandEvent& event) {
	if(!dialogAddKanjiByFreq)
		dialogAddKanjiByFreq = new DialogAddKanjiByFreq(this);
	int result = dialogAddKanjiByFreq->ShowModal();
	if(result==wxID_OK) {
		/*printf("Selected frequencies: %d to %d\n",
			dialogAddKanjiByFreq->GetLowFreq(),
			dialogAddKanjiByFreq->GetHighFreq()
		);*/
		result = jben->kanjiList->AddByFrequency(
			dialogAddKanjiByFreq->GetLowFreq(),
			dialogAddKanjiByFreq->GetHighFreq());
		this->Redraw();
		wxMessageBox(wxString::Format(
			_T("Added %d kanji to the list."), result),
			_T("Add Kanji by Frequency"), wxOK | wxICON_INFORMATION, this);
	} else if(result==wxID_CANCEL) {
		/* Do nothing */
	}
#ifdef DEBUG
	else printf("Unknown key (%08X) pressed within dialog!\n", result);
#endif
}

void FrameMainGUI::OnKanjiSaveToFile(wxCommandEvent& event) {
	/* NOTE: We may add the flag wxFD_CHANGE_DIR later, if we add in code
	   to save the full path for the jben.cfg file. */
	wxFileDialog *fd = new wxFileDialog(
		this, _T("Save Kanji List to File"), wxEmptyString, wxEmptyString,
		_T("*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if(fd->ShowModal()==wxID_OK) {
		wxString kanjiList = jben->kanjiList->ToString(20);
		wxString filename = fd->GetPath();
		/* Open file/write to file/close file */ /* TO DO */
		wxFFile f(filename, _T("w"));
		if(f.IsOpened()) {
			f.Write(kanjiList, *wxConvCurrent);
			f.Close();
		}
	}
	fd->Destroy();
}

void FrameMainGUI::OnKanjiClearList(wxCommandEvent& event) {
	jben->kanjiList->Clear();
	this->Redraw();
}

#ifdef DEBUG
void FrameMainGUI::OnKanjiDumpList(wxCommandEvent& event) {
	printf("Current kanji list: %ls\n", jben->kanjiList->ToString().c_str());
}
#endif

void FrameMainGUI::OnHelpAbout(wxCommandEvent& event) {
	wxMessageBox(_T(
		PROGRAM_NAME " v" VERSION_STR
		"\nBy " AUTHOR_NAME
		"\nCopyright " COPYRIGHT_DATE "\n\n"

		"Inspired in large by JWPce and JFC by Glenn Rosenthal:\n"
		"http://www.physics.ucla.edu/~grosenth/\n\n"

		"Powered by Monash University's EDICT2 and KANJIDIC:\n"
		"http://www.csse.monash.edu.au/~jwb/edict_doc.html\n"
		"http://www.csse.monash.edu.au/~jwb/kanjidic.html\n\n"

	    "Built using wxWidgets: http://www.wxwidgets.org/\n\n"

		"Hand writing recognition is based upon code from im-ja:\n"
		"http://im-ja.sourceforge.net/\n"
		"Which uses code based upon KanjiPad by Owen Taylor:\n"
		"http://fishsoup.net/software/kanjipad/\n\n"

		"See \"Help->License Information...\" for important license details."),
		_T("About " PROGRAM_NAME), wxOK | wxICON_INFORMATION, this);
}

void FrameMainGUI::OnHelpLicense(wxCommandEvent& event) {
	wxMessageBox(_T(
		"Program distributed under the GNU General Public License (GPL) version 2:\n"
		"http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt\n\n"
		"EDICT2 and KANJIDIC distributed under a separate license specified at\n"
		"http://www.csse.monash.edu.au/~jwb/edrdg/license.htm\n\n"

		"The SKIP (System of Kanji Indexing by Patterns) system for ordering kanji "
		"was developed by Jack Halpern (Kanji Dictionary Publishing Society at "
		"http://www.kanji.org/), and is used with his permission.\n\n"

		"Copies of the GNU General Public License, Monash University's license for "
		"the dictionary files and documentation for the dictionary files are "
		"contained in this program's \"license\" directory."),
		_T("License Information"), wxOK | wxICON_INFORMATION, this);
}

void FrameMainGUI::OnMajorTabChanged(wxNotebookEvent& event) {
#ifdef DEBUG
	printf("DEBUG: OnMAJORTabChanged called.\n");
#endif
	tabMajor = (wxNotebookPage *) tabsMain->GetPage(event.GetSelection());
	tabMinor = ((wxNotebook *)tabMajor)->GetCurrentPage();
#ifdef DEBUG
	printf("tabMajor = %p, tabMinor = %p\n", tabMajor, tabMinor);
#endif
	this->Redraw();
}

void FrameMainGUI::OnMinorTabChanged(wxNotebookEvent& event) {
#ifdef DEBUG
	printf("DEBUG: OnMinorTabChanged called.\n");
#endif
	wxNotebook *nb = (wxNotebook *) tabsMain->GetCurrentPage();
	if(nb) {
		tabMinor = (wxNotebookPage *) nb->GetPage(event.GetSelection());
	} else tabMinor = NULL;
#ifdef DEBUG
	printf("tabMajor = %p, tabMinor = %p\n", tabMajor, tabMinor);
#endif
	this->Redraw();
}

/* This is the handler code for our tab changes.
   It's called by both OnTabChanging functions.
   Returns true if the tab change should take place, and false if it
   should be vetoed. */
bool FrameMainGUI::TabChangeHandler(wxNotebookPage *page) {
	int result;
	if(page == panelConfig) {
		if(panelConfig->ChangeDetected()) {
			result = wxMessageBox(
				_T("You have made changes to your configuration.  Do you want "
				   "to save them?"),
				_T("Unsaved Changes Detected!"),
				wxYES_NO | wxCANCEL | wxICON_QUESTION, this);
			if(result==wxYES) {
				panelConfig->Commit();
			} else if(result==wxNO) {
				panelConfig->Revert();
			} else if(result==wxCANCEL) {
				return false;
			}
		}
	}
	else if(page == panelKanjiListEditor) {
		if(panelKanjiListEditor->ChangeDetected()) {
			result = wxMessageBox(
				_T("You have made changes to the kanji list.  Do you want to "
				   "save them?"),
				_T("Unsaved Changes Detected!"),
				wxYES_NO | wxCANCEL | wxICON_QUESTION, this);
			if(result==wxYES) {
				panelKanjiListEditor->Commit();
			} else if(result==wxNO) {
				panelKanjiListEditor->Revert();
			} else if(result==wxCANCEL) {
				return false;
			}
		}
	}
	else if(page == panelVocabListEditor) {
		if(panelVocabListEditor->ChangeDetected()) {
			result = wxMessageBox(
				_T("You have made changes to the vocab list.  Do you want to "
				   "save them?"),
				_T("Unsaved Changes Detected!"),
				wxYES_NO | wxCANCEL | wxICON_QUESTION, this);
			if(result==wxYES) {
				panelVocabListEditor->Commit();
			} else if(result==wxNO) {
				panelVocabListEditor->Revert();
			} else if(result==wxCANCEL) {
				return false;
			}
		}
	}
	else if(page == panelKanjiDrill) {
		if(panelKanjiDrill->TestInProgress()) {
			result =
				wxMessageBox(_T("A test is in progress.  Switching tabs will "
								"abort the test.  Are you sure you want to do "
								"this?"),
				_T("Test in progress!"), wxYES_NO | wxICON_EXCLAMATION, this);
			if(result==wxYES) {
				panelKanjiDrill->Stop();
			} else {
				return false;
			}
		}
	}
	return true;
}

void FrameMainGUI::OnTabChanging(wxNotebookEvent& event) {
	int index = event.GetOldSelection();
	if(index==-1) return;

	if(!TabChangeHandler(tabMinor)) event.Veto();
}

void FrameMainGUI::OnKanjiSortByGrade(wxCommandEvent& ev) {
	jben->kanjiList->Sort(ST_GRADE);
	this->Redraw();
}

void FrameMainGUI::OnKanjiSortByFreq(wxCommandEvent& ev) {
	jben->kanjiList->Sort(ST_FREQUENCY);
	this->Redraw();
}

void FrameMainGUI::OnKanjiSearchKanjiPad(wxCommandEvent& ev) {
	/* Multiple instances are allowed - just spin off an instance. */
	FrameKanjiPad *kanjiPad =
		new FrameKanjiPad(this, -1, _T("J-Ben: KanjiPad"));
	kanjiPad->Show();
}

void FrameMainGUI::Redraw() {
	bool state = (jben->kanjiList->Size()>0);
	kanjiMenu->Enable(ID_menuKanjiSaveToFile, state);
	kanjiMenu->Enable(ID_menuKanjiClearList, state);

	if(tabMinor) {
		((RedrawablePanel *)tabMinor)->Redraw();
	}
}

void FrameMainGUI::OnClose(wxCloseEvent& event) {
	if(!event.CanVeto()) {
#ifdef DEBUG
		fprintf(stderr, "event.CanVeto() returned false!\n");
#endif
		this->Destroy();
	}
	/* Check for active kanji drill */
	if(tabsMain->GetCurrentPage()==panelKanjiDrill &&
	  panelKanjiDrill->TestInProgress()) {
		int result =
			wxMessageBox(_T("A test is in progress.  Are you sure you want to "
							"quit?"),
			_T("Test in progress!"), wxYES_NO | wxICON_EXCLAMATION, this);
		if(result==wxYES) {
			panelKanjiDrill->Stop();
		} else {
			event.Veto();
			return;
		}
	}

	/* Everything is okay - close the program. */
	this->Destroy();
}
