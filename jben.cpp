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
#include "encoding_convert.h"

#include <cstdlib>
#ifndef __WXMSW__
#include <locale.h>
#endif

JBen *jben;

/* The application entry point */
IMPLEMENT_APP(JBen)

bool JBen::OnInit() {
	jben = this;
	kanjiList = (KanjiList *)NULL;
	vocabList = (VocabList *)NULL;
	gui = (MainGUI *)NULL;

	/* Set our wide character type */
	if(sizeof(wchar_t)==2)
		wcType = "UCS-2LE";
	else
		wcType = "UCS-4LE";
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

	/* Dictionary loading, etc., depends on our config file. */
	Preferences *prefs = Preferences::GetPrefs();
	if(!prefs) {
		/* Add more graceful handling later.  No prefs file found should just
		   cause a prefs file to be written, rather than aborting. */
		fprintf(stderr, "Could not create preferences object.  FATAL ERROR!\n\n");
		return false;
	}

	const KDict* kd = KDict::GetKDict();
	const WDict* wd = WDict::GetWDict();

	if(wd->MainDataLoaded()) {
		vocabList = new VocabList();
		vocabList->AddList(prefs->vocabList);
	}
	else vocabList = NULL;
	if(kd->MainDataLoaded()) {
		kanjiList = new KanjiList(kd->GetHashTable());
		kanjiList->AddFromString(prefs->kanjiList);
	}
	else kanjiList = NULL;

	gui = new MainGUI();
	gui->Show(true);
	SetTopWindow(gui);

	return true;
}

int JBen::OnExit() {
#ifdef DEBUG
	printf("JBen::OnExit being processed...\n");
#endif
	KDict::Destroy();
	WDict::Destroy();
	Preferences::Destroy();
	if(kanjiList) delete kanjiList;
	if(vocabList) delete vocabList;
#ifdef DEBUG
	printf("Terminating program.\n");
#endif
	return 0;
}
