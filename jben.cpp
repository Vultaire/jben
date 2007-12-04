/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: jben.cpp

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

#include "global.h"
#include "dictload.h"
#include "jben.h"

#include <stdlib.h>

JBen *jben;
Preferences *prefs;

/* The application entry point */
IMPLEMENT_APP(JBen)

bool JBen::OnInit() {
	int result;

	jben = this;
	edict = (Edict *)NULL;
	kdict = (KanjiDic *)NULL;
	kanjiList = (KanjiList *)NULL;
	vocabList = (VocabList *)NULL;
	gui = (MainGUI *)NULL;

	/* Start our random number generator */
	srand(time(NULL));
	for(int i=0;i<50;i++) rand(); /* On some platforms I've seen rand() behave
	                                 fairly predictably for the first iteration or so.
	                                 That's why I spin off a few iterations of rand()
	                                 before really using it. */

	/* DictionaryLoader was intended to be a splash screen for loading the dictionaries.  However,
	   currently this is a fast enough operation that I decided it was unnecessary. */
#if 0
	/* Display splash screen which shows progress bars while loading
	   dictionaries.  (Yes, this is fluff.) */
	DictionaryLoader *d = new DictionaryLoader();
	d->Show(true);
	SetTopWindow(d);
	result = d->LoadDictionaries();
	d->Destroy();
#endif
	/* For now, DictionaryLoader is a non-GUI class.  Just make the object, run the function, and be done with it. */
	DictionaryLoader *d = new DictionaryLoader();
	result = d->LoadDictionaries();
	delete d;

	if(result==DL_SUCCESS) {
		kanjiList = new KanjiList(kdict->GetHashTable());
		vocabList = new VocabList();
		prefs = new Preferences(_T("jben.cfg"));
		/* Show the main GUI. */
		gui = new MainGUI();
		/* On Linux, for some reason there's a placeholder "loading" mini-window which pops up during creation of this object.  Why??*/
		gui->Show(true);
		SetTopWindow(gui);
		return true;
	} else {
		wxMessageBox(wxString::Format(_T("Error during startup, DictionaryLoader result = 0x%X\n"), result),
			_T("DictionaryLoader error"), wxOK | wxICON_ERROR);
		return false;
	}
}

int JBen::OnExit() {
#ifdef DEBUG
	printf("JBen::OnExit being processed...\n");
#endif
	if(prefs) delete prefs;
	if(edict) delete edict;
	if(kanjiList) delete kanjiList;
	if(kdict) delete kdict;
#ifdef DEBUG
	printf("Terminating program.\n");
#endif
	return 0;
}
