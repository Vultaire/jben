/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: preferences.cpp

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

#include "preferences.h"
#include "jben.h"
#include "file_utils.h"
#include "encoding_convert.h"
#include "string_utils.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

Preferences* Preferences::prefsSingleton = NULL;

Preferences *Preferences::GetPrefs() {
	if(!prefsSingleton) {
		prefsSingleton = new Preferences;
/*		if(!prefsSingleton->LoadFile(".jben") &&
		   !prefsSingleton->LoadFile("jben.cfg"))
		   Destroy();*/
		if(prefsSingleton->LoadFile(".jben")    != REF_SUCCESS &&
		   prefsSingleton->LoadFile("jben.cfg") != REF_SUCCESS)
			prefsSingleton->SetDefaultPrefs();
	}
	return prefsSingleton;
}

Preferences::Preferences() {
	/* Nothing to be done */
}

void Preferences::Destroy() {
	if(prefsSingleton) {
		delete prefsSingleton;
		prefsSingleton = NULL;
	}
}

void Preferences::SetDefaultPrefs() {
	kanjidicOptions =
		KDO_READINGS | KDO_MEANINGS | KDO_HIGHIMPORTANCE | KDO_VOCABCROSSREF;
	kanjidicDictionaries = 0;
#ifdef __WXMSW__
	cfgFile = "jben.cfg";
#else
	cfgFile = ".jben";
#endif
}

int Preferences::LoadFile(const char *filename) {
	cfgFile = GetCWD().append(1, DIRSEP).append(filename);
	wstring s;
	kanjidicOptions = KDO_ALL | KDO_UNHANDLED;
	kanjidicDictionaries = 0;  /* KDD_ALL */

	int e = ReadEncodedFile(filename, s);
	if(e==REF_SUCCESS) {
		/* Split into strings for each line */
		list<wstring> tokenList = StrTokenize<wchar_t>(s, L"\n");
		wstring token, subToken;
		size_t index;
		while(tokenList.size()>0) {
			token = tokenList.front();
			tokenList.pop_front();
			if( (token.length()>0) && (token[0]!=L'#') ) {
				/* We only want to split the string into two subtokens, so we'll
			   	do it manually. */
				index = token.find_first_of(_T(" \t"));
				if(index!=wstring::npos) {
					subToken = token.substr(0, index);
					subToken = ToLower(subToken);

					if(subToken==L"kanjidicoptionmask") {
						subToken = token.substr(index+1);
						subToken = Trim(subToken);
						/*subToken.ToULong(&kanjidicOptions, 0);*/
						/* We know the subtoken starts with 0x, so skip the
						   first two characters. */
						wistringstream(subToken.substr(2))
							>> hex >> kanjidicOptions;

					} else if(subToken==L"kanjidicdictionarymask") {
						subToken = token.substr(index+1);
						subToken = Trim(subToken);
						/*subToken.ToULong(&kanjidicDictionaries, 0);*/
						wistringstream(subToken.substr(2))
							>> hex >> kanjidicDictionaries;

					} else if(subToken==L"kanjilist") {
						subToken = token.substr(index+1);
						subToken = Trim(subToken);
						kanjiList = subToken;
					} else if(subToken==L"vocablist") {
						subToken = token.substr(index+1);
						subToken = Trim(subToken);
						list<wstring> tSub
							= StrTokenize<wchar_t>(subToken, L";");
						while(tSub.size()>0) {
							subToken = tSub.front();
							tSub.pop_front();
							if(subToken.length()>0) {
								if(vocabList.length()>0)
									vocabList.append(1, L'\n');
								vocabList.append(subToken);
							}
						}

					} else if(subToken==L"edict") {
						subToken = token.substr(index+1);
						stringOpts[L"edict"]
							= Trim(subToken);

					} else if(subToken==L"kanjidic") {
						subToken = token.substr(index+1);
						stringOpts[L"kanjidic"]
							= Trim(subToken);

					} else if(subToken==L"kradfile") {
						subToken = token.substr(index+1);
						stringOpts[L"kradfile"]
							= Trim(subToken);

					} else if(subToken==L"radkfile") {
						subToken = token.substr(index+1);
						stringOpts[L"radkfile"]
							= Trim(subToken);

					} else {
						/* Unhandled - do nothing */
					}
				} else {
					/* No space/tab was found.  Check no-arg options, if any.
					   (Currently there are none.) */
				}
			} /* if(tokenlen>0, token[0]!=# */
		} /* while(hasmoretokens) */
	} /* if(file opened) */
	return e;
}

Preferences::~Preferences() {
	string prefs = GetPrefsStr();
	ofstream ofile(cfgFile.c_str(), ios::out | ios::binary);
	if(ofile.is_open()) {
		ofile << prefs;
	}
	ofile.close();
}

string Preferences::GetPrefsStr() {
	ostringstream prefs;
	string kanjiList, vocabList;

	/* Get kanji and vocab lists in UTF-8 encoded strings */
	/* In our encoding switcher, we should set a global widechar string
	   containing "UCS-2" or "UCS-4", keeping us from continually rewriting
	   this code below. */
	/* I've rewritten the code, but now we need the variable it depends upon. */
	kanjiList = ConvertString<wchar_t, char>(
		jben->kanjiList->ToString(),
		wcType.c_str(), "UTF-8");
	vocabList = ConvertString<wchar_t, char>(
		jben->vocabList->ToString(';'),
		wcType.c_str(), "UTF-8");

	prefs << "KanjidicOptionMask 0x"
		  << uppercase << hex << setw(8) << setfill('0')
		  << kanjidicOptions << '\n';
	prefs << "KanjidicDictionaryMask 0x"
		  << uppercase << hex << setw(8) << setfill('0')
		  << kanjidicDictionaries << '\n';
	prefs << "KanjiList "
		  << ConvertString<wchar_t,char>
		(jben->kanjiList->ToString(),wcType.c_str(),"UTF-8")
		  << '\n';
	prefs << "VocabList "
		  << ConvertString<wchar_t,char>
		(jben->vocabList->ToString(';'),wcType.c_str(),"UTF-8")
		  << '\n';

	return prefs.str();
}
