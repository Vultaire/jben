/*
Project: J-Ben
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File:         preferences.cpp
Author:       Paul Goins
Contributors: Alain Bertrand

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

#define CURRENT_CONFIG_VERSION "1.2.1"

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

		/* Check the parent directory first.  (This is intended for mobile installs.) */
		bool OK = (prefsSingleton->LoadFile(".." DSSTR CFGFILE) == REF_SUCCESS);

		/* If it loaded, check the save target.
		   If the target is "home", AND the home directory was found,
		   AND a config file is present there, then reset to default settings
		   and re-load from that file instead. */
		if(OK && prefsSingleton->GetSetting("config_save_target")=="home") {
			if((!sCfgPath.empty()) && FileExists(sCfgPath.c_str())) {
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
			OK = (prefsSingleton->LoadFile(sCfgPath.c_str()) == REF_SUCCESS);
			if(OK) prefsSingleton->GetSetting("config_save_target") = "home";
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
	/* Define default paths to supported (and future supported) dicts.
	   J-Ben will automatically append ".gz" and load compressed dictionaries
	   if found. */
	/* Identifiers are of the form "jben_obj.dict_type.file[#]".  Dicts with the
	   same format should share the same dict_type and add a file number. */
	stringOpts["kdict.kanjidic2.file"]
		= JB_DATADIR DSSTR "dicts" DSSTR "kanjidic2.xml";
	stringOpts["kdict.kanjidic.file"]
		= JB_DATADIR DSSTR "dicts" DSSTR "kanjidic";
	stringOpts["kdict.kanjidic.file2"]
		= JB_DATADIR DSSTR "dicts" DSSTR "kanjd212";
	stringOpts["kdict.kradfile.file"]
		= JB_DATADIR DSSTR "dicts" DSSTR "kradfile";
	stringOpts["kdict.radkfile.file"]
		= JB_DATADIR DSSTR "dicts" DSSTR "radkfile";
	stringOpts["wdict.edict.file"]
		= JB_DATADIR DSSTR "dicts" DSSTR "edict";
	stringOpts["wdict.edict2.file"]
		= JB_DATADIR DSSTR "dicts" DSSTR "edict2";
	/* J-Ben's internal encoding is UTF-8, however most of Jim Breen's non-XML
	   dict files are in EUC-JP.  We should allow the program to support
	   these files. */
	stringOpts["wdict.edict.file.encoding"] = "euc-jp";
	stringOpts["wdict.edict2.file.encoding"] = "euc-jp";
	stringOpts["kdict.kanjidic.file.encoding"] = "euc-jp";
	stringOpts["kdict.kanjidic.file2.encoding"] = "euc-jp";
	stringOpts["kdict.kradfile.file.encoding"] = "euc-jp";
	stringOpts["kdict.radkfile.file.encoding"] = "euc-jp";
	/* Specify JIS encoding for kanjidic files (jis-208 or jis-212) */
	/* stringOpts["kdict.kanjidic.file.jispage"]; */
	stringOpts["kdict.kanjidic.file2.jispage"] = "jis212";

	stringOpts["sod_dir"] = JB_DATADIR DSSTR "sods";

	stringOpts["kanjitest.writing.showonyomi"]="true" ;
	stringOpts["kanjitest.writing.showkunyomi"]="true" ;
	stringOpts["kanjitest.writing.showenglish"]="true" ;
	stringOpts["kanjitest.reading.showonyomi"]="false" ;
	stringOpts["kanjitest.reading.showkunyomi"]="false" ;
	stringOpts["kanjitest.reading.showenglish"]="false" ;
	stringOpts["kanjitest.showanswer"]="1";
	stringOpts["kanjitest.correctanswer"]="2";
	stringOpts["kanjitest.wronganswer"]="3";
	stringOpts["kanjitest.stopdrill"]="4";
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
		while(!tokenList.empty()) {
			token = tokenList.front();
			tokenList.pop_front();
			if((!token.empty()) && (token[0]!=L'#') ) {
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
						while(!tSub.empty()) {
							subToken = tSub.front();
							tSub.pop_front();
							if(!subToken.empty()) {
								string *temp = &stringOpts["vocablist"];
								if(!temp->empty())
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
			<< "\" could NOT be loaded.";
		el.Push(EL_Silent, oss.str());
	}

	/* The file is now loaded.
	   Upgrade it to the latest version, if necessary. */
	if(stringOpts["config_version"] != CURRENT_CONFIG_VERSION)
		UpgradeConfigFile();

	return e;
}

void Preferences::UpgradeConfigFile() {
	string& version = stringOpts["config_version"];
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
	if(version=="1.1") {
		/* 1.1 to 1.2:
			- Convert xdict_filename to xdict.dicttype_file# format */
		string* pStr;
		pStr = &stringOpts["kdict_kanjidic"];
		if(!pStr->empty()) {
			stringOpts["kdict.kanjidic.file"] = *pStr;
			pStr->clear();
		}
		pStr = &stringOpts["kdict_kanjd212"];
		if(!pStr->empty()) {
			stringOpts["kdict.kanjidic.file2"] = *pStr;
			stringOpts["kdict.kanjidic.file2.jispage"] = "jis212";
			pStr->clear();
		}
		pStr = &stringOpts["kdict_kanjidic2"];
		if(!pStr->empty()) {
			stringOpts["kdict.kanjidic2.file"] = *pStr;
			pStr->clear();
		}
		pStr = &stringOpts["kdict_kradfile"];
		if(!pStr->empty()) {
			stringOpts["kdict.kradfile.file"] = *pStr;
			pStr->clear();
		}
		pStr = &stringOpts["kdict_radkfile"];
		if(!pStr->empty()) {
			stringOpts["kdict.radkfile.file"] = *pStr;
			pStr->clear();
		}
		pStr = &stringOpts["wdict_edict2"];
		if(!pStr->empty()) {
			stringOpts["wdict.edict2.file"] = *pStr;
			pStr->clear();
		}
		version = "1.2";
	}
	if (version=="1.2"){
		/* New items were added, but nothing needs to be changed here.
		 However, since this section was already put in, we gotta keep
		at least the version change in for proper compatibility. */
		version = "1.2.1";
	}

	version = CURRENT_CONFIG_VERSION;
}

Preferences::~Preferences() {
	/* If the listmanager wasn't allowed to load (rare), we don't want to
	   potentially overwrite a good config file with a bad one. */
	if(ListManager::Exists()) {
		string prefs = GetPrefsStr();
		list<string> fileNames;

		/* Set output file names */
		if(ToLower(stringOpts["config_save_target"]) == "home") {
			fileNames.push_back(string(getenv(HOMEENV)).append(1, DSCHAR)
								.append(CFGFILE));
			if(originalSaveTarget == "mobile")
				fileNames.push_back(".." DSSTR CFGFILE);
		} else {
			fileNames.push_back(".." DSSTR CFGFILE);
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
						(boost::format
						 ("Preferences file \"%s\" saved successfully.")
						 % s).str());
				ofile.close();
			} else {
				el.Push(EL_Error,
					(boost::format("Unable to save preferences to file \"%s\"!")
					 % CFGFILE).str());
			}
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
	if(ListManager::Exists()) {
		ListManager* lm = ListManager::Get();
		KanjiList* kl = lm->KList();
		VocabList* vl = lm->VList();
		if(kl) kanjiList = utfconv_wm(kl->ToString());
		if(vl) vocabList = utfconv_wm(vl->ToString(';'));

		prefs << "KanjidicOptionMask\t0x"
			  << uppercase << hex << setw(8) << setfill('0')
			  << kanjidicOptions << '\n';
		prefs << "KanjidicDictionaryMask\t0x"
			  << uppercase << hex << setw(8) << setfill('0')
			  << kanjidicDictionaries << '\n';
		prefs << "KanjiList\t" << kanjiList << '\n';
		prefs << "VocabList\t" << vocabList << '\n';
	}

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
		else if(lKey=="gui.wnd.kanjiksearch.size")
			defaultValue = "-1x400";
		else if(lKey=="gui.wnd.kanjiksearch.pane.divider.pos")
			defaultValue = "300";

#ifdef __WIN32__
		/* GTK does not support bold text under Windows very well.  It might be
		   due to the rendering engine it use, but I'm unsure.  Italic and
		   normal text work fine, but MANY Windows fonts cannot be selected as
		   bold under GTK+. */
		/* Tahoma seems to show Japanese characters and bold English text okay.
		   It's a tolerable compromise to -proper- GTK font support. */
		else if(lKey=="font.en")       defaultValue = "Tahoma 12";
		else if(lKey=="font.en.small") defaultValue = "Tahoma 8";
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

#include <iostream>
list<string> Preferences::GetKeyList(string searchKey) {
	list<string> l;
	map<string,string>::iterator it, lb, ub;
	/* Lower bound is easy to find */
	lb = stringOpts.lower_bound(searchKey);
	/* Upper bound must be manually tested */
	for(ub=lb; ub!=stringOpts.end(); ub++) {
		if(ub->first.compare
		   (0, min(ub->first.length(), searchKey.length()), searchKey) != 0)
			break;
	}

	for(it=lb; it!=ub; it++)
		l.push_back(it->first);
	return l;
}

typedef std::pair<string,string> pair_str;
list<string> Preferences::GetKeyList(boost::regex exp) {
	list<string> l;
	foreach(const pair_str& p, stringOpts) {
		if(regex_match(p.first, exp)) l.push_back(p.first);
	}
	return l;
}
