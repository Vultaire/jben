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

#ifndef JB_DATADIR
	#define JB_DATADIR "."
#endif

#include "preferences.h"
#include "jben.h"
#include "file_utils.h"
#include "encoding_convert.h"
#include "string_utils.h"
#include "errorlog.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

Preferences* Preferences::prefsSingleton = NULL;

Preferences *Preferences::Get() {
	if(!prefsSingleton) {
		prefsSingleton = new Preferences;
		/* Load with default preferences, and overwrite with any stored
		   preferences. */
		prefsSingleton->SetDefaultPrefs();
		/* Try to load .jben, followed by jben.cfg if the first fails. */
#ifdef __WXMSW__
		string homedir = getenv("AppData");
		homedir.append(1, '\\');
#else
		string homedir = getenv("HOME");
		homedir.append(1, '/');
#endif
		/* Check for the config file in the user's home directory. */
		bool OK = (prefsSingleton
				   ->LoadFile(string(homedir).append(".jben").c_str())
				   == REF_SUCCESS);
		if(!OK)
			OK = (prefsSingleton
				  ->LoadFile(string(homedir).append("jben.cfg").c_str())
				  == REF_SUCCESS);
		/* If the config file could not be loaded from the home directory,
		   fall back and check the current directory. */
		if(!OK)
			OK = (prefsSingleton->LoadFile(".jben") == REF_SUCCESS);
		if(!OK)
			prefsSingleton->LoadFile("jben.cfg");
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
	stringOpts["config_version"] = "1";
#ifdef __WXMSW__
	cfgFile = getenv("AppData");
	if(cfgFile.length()==0) {
		el.Push(EL_Error, "Could not retrieve home directory.");
		cfgFile = GetCWD();
	}
	cfgFile.append("\\jben.cfg");
	stringOpts["kdict_kanjidic"] = JB_DATADIR "\\dicts\\kanjidic";
	stringOpts["kdict_kradfile"] = JB_DATADIR "\\dicts\\kradfile";
	stringOpts["kdict_radkfile"] = JB_DATADIR "\\dicts\\radkfile";
	stringOpts["wdict_edict2"]   = JB_DATADIR "\\dicts\\edict2";
	stringOpts["sod_dir"]        = JB_DATADIR "\\sods";
#else
	cfgFile = getenv("HOME");
	if(cfgFile.length()==0) {
		el.Push(EL_Error, "Could not retrieve home directory.");
		cfgFile = GetCWD();
	}
	cfgFile.append("/.jben");
	stringOpts["kdict_kanjidic"] = JB_DATADIR "/dicts/kanjidic";
	stringOpts["kdict_kradfile"] = JB_DATADIR "/dicts/kradfile";
	stringOpts["kdict_radkfile"] = JB_DATADIR "/dicts/radkfile";
	stringOpts["wdict_edict2"]   = JB_DATADIR "/dicts/edict2";
	stringOpts["sod_dir"]        = JB_DATADIR "/sods";
#endif


}

int Preferences::LoadFile(const char *filename) {
	wstring s;
	kanjidicOptions = KDO_ALL | KDO_UNHANDLED;
	kanjidicDictionaries = 0;  /* KDD_ALL */

	int e = ReadEncodedFile(filename, s);
	if(e==REF_SUCCESS) {
		cfgFile = filename;
#ifdef DEBUG
		cout << "Preferences file \"" << filename
			 << "\" loaded successfully." << endl;
#endif
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
						/* We know the subtoken starts with 0x, so skip the
						   first two characters. */
						wistringstream(subToken.substr(2))
							>> hex >> kanjidicOptions;

					} else if(subToken==L"kanjidicdictionarymask") {
						subToken = token.substr(index+1);
						subToken = Trim(subToken);
						wistringstream(subToken.substr(2))
							>> hex >> kanjidicDictionaries;

					} else if(subToken==L"vocablist") {
						subToken = token.substr(index+1);
						subToken = Trim(subToken);
						list<wstring> tSub
							= StrTokenize<wchar_t>(subToken, L";");
						while(tSub.size()>0) {
							subToken = tSub.front();
							tSub.pop_front();
							if(subToken.length()>0) {
								string *temp = &stringOpts["vocablist"];
								if(temp->length()>0)
									temp->append(1, '\n');
								temp->append(utfconv_wm(subToken));
							}
						}

					} else {
						/* Default handling for any
						   other string-based entries */
						string key = utfconv_wm(subToken);
						subToken = token.substr(index+1);						
						stringOpts[key] = utfconv_wm(Trim(subToken));
					}
				} else {
					/* No space/tab was found.  Check no-arg options, if any.
					   (Currently there are none.) */
				}
			} /* if(tokenlen>0, token[0]!=# */
		} /* while(hasmoretokens) */
	} /* if(file opened) */
	else {
		ostringstream oss;
		oss << "Preferences file \"" << filename
			<< "\" could NOT be loaded." << endl;
		el.Push(EL_Silent, oss.str());
	}
	return e;
}

Preferences::~Preferences() {
	string prefs = GetPrefsStr();
	ofstream ofile(cfgFile.c_str(), ios::out | ios::binary);
	if(ofile.is_open()) {
#ifdef DEBUG
		cout << "Preferences saved to file \"" << cfgFile
			 << "\"." << endl;
#endif
		ofile << prefs;
	}
	else {
		ostringstream oss;
		oss << "Error: Unable to save preferences to file \"" << cfgFile
			<< "\"!" << endl;
		el.Push(EL_Error, oss.str());
	}
	ofile.close();
}

string Preferences::GetPrefsStr() {
	ostringstream prefs;
	string kanjiList, vocabList;

	/* Output config version first */
	prefs << "config_version\t" << stringOpts["config_version"] << '\n';

	/* Get kanji and vocab lists in UTF-8 encoded strings */
	kanjiList = utfconv_wm(jben->kanjiList->ToString());
	vocabList = utfconv_wm(jben->vocabList->ToString(';'));

	prefs << "KanjidicOptionMask\t0x"
		  << uppercase << hex << setw(8) << setfill('0')
		  << kanjidicOptions << '\n';
	prefs << "KanjidicDictionaryMask\t0x"
		  << uppercase << hex << setw(8) << setfill('0')
		  << kanjidicDictionaries << '\n';
	prefs << "KanjiList\t"
		  << utfconv_wm(jben->kanjiList->ToString()) << '\n';
	prefs << "VocabList\t"
		  << utfconv_wm(jben->vocabList->ToString(';')) << '\n';

	/* Append any other variables stored */
	for(map<string, string>::iterator mi = stringOpts.begin();
		mi != stringOpts.end(); mi++) {
		if(mi->first!="kanjilist" &&
		   mi->first!="vocablist" &&
		   mi->first!="config_version") {
			prefs << mi->first << '\t' << mi->second << '\n';
		}
	}

	return prefs.str();
}

string& Preferences::GetSetting(string key) {
	return stringOpts[key];
}
