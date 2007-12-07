/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: dictload.cpp

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

#include "dictload.h"
#include "global.h"
#include "kanjidic.h"
#include "edict.h"

#if 0
#include "version.h"
/* Splash window style constructor */
DictionaryLoader::DictionaryLoader() :
  wxFrame((wxFrame *)NULL, -1, _T("Starting " PROGRAM_NAME), wxPoint(-1,-1),
  wxSize(), wxDEFAULT_FRAME_STYLE) {
	/* Do nothing special */
}
#endif

DictionaryLoader::DictionaryLoader() {
	/* Do nothing special */
}

int DictionaryLoader::LoadDictionaries() {
	int result;
	jben->kdict = KanjiDic::LoadKanjiDic("kanjidic", result);
	if(result != KD_SUCCESS) return DL_KANJIDIC_NOT_FOUND;
	jben->edict = Edict::LoadEdict("edict2", result);
	if(result != ED_SUCCESS) return DL_EDICT_NOT_FOUND;

	return DL_SUCCESS;
}
