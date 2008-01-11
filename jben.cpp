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

#include "global.h"
#include "jben.h"

#include <stdlib.h>

JBen *jben;

/* The application entry point */
IMPLEMENT_APP(JBen)

bool JBen::OnInit() {
	jben = this;
	kanjiList = (KanjiList *)NULL;
	vocabList = (VocabList *)NULL;
	gui = (MainGUI *)NULL;

	/* Start our random number generator */
	srand(time(NULL));
	for(int i=0;i<50;i++) rand(); /* On some platforms I've seen rand() behave
	                                 fairly predictably for the first iteration
									 or so.  That's why I spin off a few
									 iterations of rand() before really using
									 it. */

	/* Dictionary loading, etc., depends on our config file. */
	g_prefs = prefs = new Preferences(_T("jben.cfg"));

	dicts = new Dictionaries();

	if(dicts->GetEdict())
		vocabList = new VocabList();
	else vocabList = NULL;
	if(dicts->GetKanjiDic())
		kanjiList = new KanjiList(dicts->GetKanjiDic()->GetHashTable());
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
	if(prefs) delete prefs;
	if(dicts) delete dicts;
	if(kanjiList) delete kanjiList;
	if(vocabList) delete vocabList;
#ifdef DEBUG
	printf("Terminating program.\n");
#endif
	return 0;
}
