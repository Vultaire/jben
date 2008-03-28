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
#	define JB_DATADIR "."
#endif

#define CURRENT_CONFIG_VERSION "1.1"

#include "preferences.h"
#include "kdict.h"
#include "listmanager.h"
#include "file_utils.h"
#include "encoding_convert.h"
#include "string_utils.h"
#include "errorlog.h"
#include "jben_defines.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

Preferences* Preferences::prefsSingleton = NULL;

Preferences *Preferences::Get() {
	if(!prefsSingleton) {
		prefsSingleton = new Preferences;

		/* Get home directory for this environment, if possible. */
		string sCfgPath;
		char *sz = getenv(HOMEENV);
		/* Create path to config file in home dir */
		if(sz) {
			sCfgPath = sz;
			sCfgPath.append(1, DSCHAR);
			sCfgPath.append(CFGFILE);
		}

		/* Load with default preferences, and overwrite with any stored
		   preferences. */
		prefsSingleton->SetDefaultPrefs();

		/* Check the local directory first.  (This is intended for mobile installs.) */
		bool OK = (prefsSingleton->LoadFile(CFGFILE) == REF_SUCCESS);

		/* If it loaded, check the save target.
		   If the target is "home", AND the home directory was found,
		   AND a config file is present there, then reset to default settings
		   and re-load from that file instead. */
		if(OK && prefsSingleton->GetSetting("config_save_target")=="home") {
			if(sCfgPath.length() > 0 && FileExists(sCfgPath.c_str())) {
				prefsSingleton->SetDefaultPrefs();
				OK = false;
			} else {
				el.Push(EL_Info,
					"J-Ben was unable to locate a standard mode config file, "
					"and will be falling back to mobile mode.  To change back "
					"to standard mode, please turn mobile mode off via the "
					"Edit->Preferences menu.");
				prefsSingleton->GetSetting("config_save_target") = "mobile";
			}
		}

		/* Finally: check for the config file in the user's home directory. */
		if(!OK) {
			string homedir = sz;
			homedir.append(1, DSCHAR);
			prefsSingleton->LoadFile(sCfgPath.c_str());
			prefsSingleton->GetSetting("config_save_target") = "home";
		}

		/* Track whether the save target has changed since the config file was loaded. */
		prefsSingleton->originalSaveTarget
			= prefsSingleton->GetSetting("config_save_target");
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
	stringOpts.clear();
	stringOpts["config_version"] = CURRENT_CONFIG_VERSION;
	stringOpts["config_save_target"] = "unset";
	stringOpts["kdict_kanjidic2"]
		= JB_DATADIR DSSTR "dicts" DSSTR "kanjidic2.xml";
	stringOpts["kdict_kanjidic"] = JB_DATADIR DSSTR "dicts" DSSTR "kanjidic";
	stringOpts["kdict_kanjd212"] = JB_DATADIR DSSTR "dicts" DSSTR "kanjd212";
	stringOpts["kdict_kradfile"] = JB_DATADIR DSSTR "dicts" DSSTR "kradfile";
	stringOpts["kdict_radkfile"] = JB_DATADIR DSSTR "dicts" DSSTR "radkfile";
	stringOpts["wdict_edict2"]   = JB_DATADIR DSSTR "dicts" DSSTR "edict2";
	stringOpts["sod_dir"]        = JB_DATADIR DSSTR "sods";
}

int Preferences::LoadFile(const char *filename) {
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
				index = token.find_first_of(L" \t");
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

		ostringstream oss;
		oss << "Preferences file \"" << filename
			<< "\" loaded successfully.";
		el.Push(EL_Silent, oss.str());

	} /* if(file opened) */
	else {
		ostringstream oss;
		oss << "Preferences file \"" << filename
			<< "\" could NOT be loaded." << endl;
		el.Push(EL_Silent, oss.str());
	}

	/* The file is now loaded.
	   Upgrade it to the latest version, if necessary. */
	if(stringOpts["config_version"] != CURRENT_CONFIG_VERSION)
		UpgradeConfigFile();

	return e;
}

void Preferences::UpgradeConfigFile() {
	string version = stringOpts["config_version"];
	/* Iterate through the version-wise changes */
	if(version=="1") {
		el.Push(EL_Silent, "Upgrading config file from version 1 to 1.1.");
		/* 1 to 1.1:
		   - Add config_save_target setting
		   - Add KANJIDIC2 and KANJD212 default settings */
		stringOpts["config_save_target"] = "home";
		stringOpts["kdict_kanjidic2"]
			= JB_DATADIR DSSTR "dicts" DSSTR "kanjidic2.xml";
		stringOpts["kdict_kanjd212"]
			= JB_DATADIR DSSTR "dicts" DSSTR "kanjd212";
		version = "1.1";
	}
	stringOpts["config_version"] = CURRENT_CONFIG_VERSION;	
}

Preferences::~Preferences() {
	string prefs = GetPrefsStr();
	list<string> fileNames;

	/* Set output file names */
	if(ToLower(stringOpts["config_save_target"]) == "home") {
		fileNames.push_back(string(getenv(HOMEENV)).append(1, DSCHAR)
							.append(CFGFILE));
		if(originalSaveTarget == "mobile") fileNames.push_back(CFGFILE);
	} else {
		fileNames.push_back(CFGFILE);
		if(originalSaveTarget == "home")
			fileNames.push_back(string(getenv(HOMEENV))
								.append(1, DSCHAR).append(CFGFILE));
	}
	
	ofstream ofile;
	foreach(string& s, fileNames) {
		ofile.open(s.c_str(), ios::out | ios::binary);
		if(ofile.is_open()) {
			ofile << prefs;
			el.Push(EL_Silent,
				(boost::format("Preferences file \"%s\" saved successfully.")
				 % s).str());
			ofile.close();
		} else {
			el.Push(EL_Error,
				(boost::format("Unable to save preferences to file \"%s\"!")
				 % CFGFILE).str());
		}
	}
}

string Preferences::GetPrefsStr() {
	ostringstream prefs;
	string kanjiList, vocabList;

	/* Output config version first */
	prefs << "config_version\t" << stringOpts["config_version"] << '\n';

	/* Get kanji and vocab lists in UTF-8 encoded strings */
	/* Currently we only save one list, "master", the currently selected one. */
	ListManager* lm = ListManager::Get();
	kanjiList = utfconv_wm(lm->KList()->ToString());
	vocabList = utfconv_wm(lm->VList()->ToString(';'));

	prefs << "KanjidicOptionMask\t0x"
		  << uppercase << hex << setw(8) << setfill('0')
		  << kanjidicOptions << '\n';
	prefs << "KanjidicDictionaryMask\t0x"
		  << uppercase << hex << setw(8) << setfill('0')
		  << kanjidicDictionaries << '\n';
	prefs << "KanjiList\t" << kanjiList << '\n';
	prefs << "VocabList\t" << vocabList << '\n';

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

string& Preferences::GetSetting(string key, string defaultValue) {
	string lKey = ToLower(key);
	map<string, string>::iterator mi = stringOpts.find(lKey);
	if(mi!=stringOpts.end()) return mi->second;
	/* Determine default value, if one is not specified */
	if(defaultValue == "") {
		if     (lKey=="gui.main.size")
			defaultValue = "600x400";
		else if(lKey=="gui.dlg.kanjilisteditor.size")
			defaultValue = "450x-1";
		else if(lKey=="gui.wnd.kanjihwpad.size")
			defaultValue = "200x230";
#ifdef __WIN32__
		else if(lKey=="font.en")       defaultValue = "Arial 12";
		else if(lKey=="font.en.small") defaultValue = "Arial 8";
		else if(lKey=="font.ja")       defaultValue = "MS Mincho 16";
		else if(lKey=="font.ja.large") defaultValue = "MS Mincho 32";
#else
		else if(lKey=="font.en")       defaultValue = "sans 12";
		else if(lKey=="font.en.small") defaultValue = "sans 8";
		else if(lKey=="font.ja")       defaultValue = "serif 16";
		else if(lKey=="font.ja.large") defaultValue = "serif 32";
#endif
	}
	/* Assign default value */
	stringOpts[lKey] = defaultValue;
	return stringOpts[lKey];
}
