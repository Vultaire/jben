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
#include "file_utils.h"
#include "wx/tokenzr.h"
#include "kanjidic.h"
#include "wx/ffile.h"
#include "global.h"

Preferences* g_prefs;

Preferences::Preferences(const wxChar *filename) {
	cfgFile = wxFileName(wxGetCwd(), filename).GetFullPath();
	wxString s;
	kanjidicOptions = KDO_ALL | KDO_UNHANDLED;
	kanjidicDictionaries = 0;  /* KDD_ALL */

	if(ReadEncodedFile(filename, _T("utf-8"), s)==REF_SUCCESS) {
		/* Split into strings for each line */
		wxStringTokenizer t(s, _T("\n"));
		wxString token, subToken;
		size_t index;
		while(t.HasMoreTokens()) {
			token = t.GetNextToken();
			if( (token.length()>0) && (token[0]!=_T('#')) ) {
				/* We only want to split the string into two subtokens, so we'll
			   	do it manually. */
				index = token.find_first_of(_T(" \t"));
				if(index!=wxString::npos) {
					subToken = token.substr(0, index);
					subToken = subToken.MakeLower();

					if(subToken==_T("kanjidicoptionmask")) {
						subToken = token.substr(index+1);
						subToken = subToken.Trim(false).Trim(true);
						subToken.ToULong(&kanjidicOptions, 0);

					} else if(subToken==_T("kanjidicdictionarymask")) {
						subToken = token.substr(index+1);
						subToken = subToken.Trim(false).Trim(true);
						subToken.ToULong(&kanjidicDictionaries, 0);

					} else if(subToken==_T("kanjilist")) {
						subToken = token.substr(index+1);
						subToken = subToken.Trim(false).Trim(true);
						jben->kanjiList->AddFromString(subToken);

					} else if(subToken==_T("vocablist")) {
						subToken = token.substr(index+1);
						subToken = subToken.Trim(false).Trim(true);
						wxStringTokenizer tSub(subToken, _T(";"));
						while(tSub.HasMoreTokens()) {
							subToken = tSub.GetNextToken();
							if(subToken.length()>0) {
								jben->vocabList->Add(subToken);
							}
						}

					} else if(subToken==_T("edict")) {
						subToken = token.substr(index+1);
						stringOpts["edict"]
							= subToken.Trim(false).Trim(true);

					} else if(subToken==_T("kanjidic")) {
						subToken = token.substr(index+1);
						stringOpts["kanjidic"]
							= subToken.Trim(false).Trim(true);

					} else if(subToken==_T("kradfile")) {
						subToken = token.substr(index+1);
						stringOpts["kradfile"]
							= subToken.Trim(false).Trim(true);

					} else if(subToken==_T("radkfile")) {
						subToken = token.substr(index+1);
						stringOpts["radkfile"]
							= subToken.Trim(false).Trim(true);

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
}

Preferences::~Preferences() {
	wxCSConv transcoder(_T("utf-8"));

	wxFFile out(cfgFile, _T("w"));
	if(out.IsOpened()) {
		out.Write(GetPreferences(), transcoder);
		out.Close();
	}
}

wxString Preferences::GetPreferences() {
	wxString prefs;
	prefs.Printf(_T("KanjidicOptionMask 0x%08lX\n"), kanjidicOptions);
	prefs.append(wxString::Format(_T("KanjidicDictionaryMask 0x%08lX\n"), kanjidicDictionaries));
	prefs.append(wxString::Format(_T("KanjiList %ls\n"), jben->kanjiList->ToString().c_str()));
	prefs.append(wxString::Format(_T("VocabList %ls\n"), jben->vocabList->ToString(_T(';')).c_str()));
	return prefs;
}
