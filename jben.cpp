/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: jben.cpp

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

#include "jben.h"
#include "wdict.h"
#include "kdict.h"
#include "preferences.h"
#include "listmanager.h"
#include "encoding_convert.h"
#include "errorlog.h"

#include <cstdlib>
#ifndef __WXMSW__
#include <locale.h>
#endif

#define DEFAULT_LIST_NAME "master"

/* The application entry point */
IMPLEMENT_APP(JBen)

JBen* jben;

void ErrorLogDisplayFunc(ELType t, const string& message, void *srcObj) {
	switch(t) {
	case EL_Error:
#ifdef DEBUG
		cout << "[Error] " << message << endl;
#endif
		wxMessageBox(utfconv_mw(message), _T("Error"),
					 wxOK | wxICON_ERROR, (wxWindow*)srcObj);
		break;
	case EL_Warning:
#ifdef DEBUG
		cout << "[Warning] " << message << endl;
#endif
		wxMessageBox(utfconv_mw(message), _T("Warning"),
					 wxOK | wxICON_ERROR, (wxWindow*)srcObj);
		break;
	case EL_Info:
#ifdef DEBUG
		cout << "[Info] " << message << endl;
#endif
		wxMessageBox(utfconv_mw(message), _T("Info"),
					 wxOK | wxICON_ERROR, (wxWindow*)srcObj);
		break;
	case EL_Silent:
#ifdef DEBUG
		cout << "[Silent] " << message << endl;
#endif
		/* do nothing */
		break;
	}
}

bool JBen::OnInit() {
	jben = this;
	KanjiList* kanjiList = NULL;
	VocabList* vocabList = NULL;
	gui = NULL;

	/* the below -might- help on win32 systems, but for now is unused. */
#if 0
#ifndef __WXMSW__
	setlocale(LC_ALL, "");
#endif
#endif
	
	/* Start our random number generator */
	srand(time(NULL));
	for(int i=0;i<50;i++) rand(); /* On some platforms I've seen rand() behave
									 fairly predictably for the first iteration
									 or so.  That's why I spin off a few
									 iterations of rand() before really using
									 it. */

	/* Various initialization */
	InitUTFConv();
	el.RegDisplayFunc(ErrorLogDisplayFunc);

	/* Dictionary loading, etc., depends on our config file. */
	Preferences *prefs = Preferences::Get();
	if(!prefs) {
		/* This -should- never occur now. */
		fprintf(stderr, "Could not create preferences object.  FATAL ERROR!\n\n");
		return false;
	}

	const KDict* kd = KDict::Get();
	const WDict* wd = WDict::Get();
	ListManager* lm = ListManager::Get();

	if(wd->MainDataLoaded()) {
		lm->AddVocabList(DEFAULT_LIST_NAME);
		lm->ChooseVocabList(DEFAULT_LIST_NAME);
		vocabList = lm->VList();
		vocabList->AddList(
			utfconv_mw(prefs->GetSetting("vocablist")));
	}
	else vocabList = NULL;
	if(kd->MainDataLoaded()) {
		lm->AddKanjiList(DEFAULT_LIST_NAME);
		lm->ChooseKanjiList(DEFAULT_LIST_NAME);
		kanjiList = lm->KList();
		kanjiList->AddFromString(
			utfconv_mw(prefs->GetSetting("kanjilist")));
	}
	else kanjiList = NULL;

	gui = new FrameMainGUI();
	gui->Show(true);
	SetTopWindow(gui);

	return true;
}

int JBen::OnExit() {
#ifdef DEBUG
	printf("JBen::OnExit being processed...\n");
#endif
	/* Careful about destruction order!
	   ~Preferences depends on ListManager! */
	Preferences::Destroy();
	ListManager::Destroy();
	KDict::Destroy();
	WDict::Destroy();
	DestroyUTFConv();
#ifdef DEBUG
	printf("Terminating program.\n");
#endif
	return 0;
}
