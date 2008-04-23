/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: kanjidic.cpp

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

#include "kdict.h"
#include "preferences.h"
#include "listmanager.h"
#include "encoding_convert.h"
#include "string_utils.h"
#include "file_utils.h"
#include "jben_defines.h"
#include "jutils.h"
#include "errorlog.h"
#include <libxml/xmlreader.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
#include <cstring>
#include <boost/foreach.hpp>
using namespace std;

#define foreach BOOST_FOREACH

#ifdef __WIN32__
#	define FALLBACK_DICTDIR "dicts\\"
#else
#	define FALLBACK_DICTDIR "dicts/"
#endif

KDict* KDict::kdictSingleton = NULL;

const KDict* KDict::Get() {
	if(!kdictSingleton)
		kdictSingleton = new KDict;
	return kdictSingleton;
}

KDict::KDict() {
	Preferences* p = Preferences::Get();
	list<string> files;
	int result;
	bool success;

	/* Load KANJIDIC2, if present. */
	files = p->GetKeyList(boost::regex("^kdict.kanjidic2.file[\\d]*$"));
	success = false;
	foreach(string& s, files) {
		result = LoadKanjidic2(p->GetSetting(s));
		if(result == KD_SUCCESS) success = true;
	}
	files.clear();

	/* If KANJIDIC2 is not present, load KANJIDIC and/or KANJD212 */
	if(!success) {
		files = p->GetKeyList(boost::regex("^kdict.kanjidic.file[\\d]*$"));
		success = false;
		foreach(string& s, files) {
			string encoding = p->GetSetting(string(s).append(".encoding"));
			string jispage = p->GetSetting(string(s).append(".jispage"));
			if(encoding.empty()) jispage = "euc-jp";
			if(jispage.empty()) jispage = "jis208";
			/* Need to add encoding option to loader */
			result = LoadKanjidic(p->GetSetting(s), jispage, encoding);
			if(result == KD_SUCCESS) success = true;
		}
	}

	/* If we STILL don't have anything loaded, check for dictionaries in the
	   ./dicts directory. */
	if(!success) {
		result = (LoadKanjidic2(FALLBACK_DICTDIR "kanjidic2.xml"));
		if(result == KD_SUCCESS) success = true;
	}
	if(!success) {
		LoadKanjidic(FALLBACK_DICTDIR "kanjidic");
		LoadKanjidic(FALLBACK_DICTDIR "kanjd212", "jis212");
	}

	/* Load supplemental dictionary files */
	result = LoadKradfile(p->GetSetting("kdict.kradfile.file"));
	if(result!=KD_SUCCESS) LoadKradfile(FALLBACK_DICTDIR "kradfile");
	result = LoadRadkfile(p->GetSetting("kdict.radkfile.file"));
	if(result!=KD_SUCCESS) LoadRadkfile(FALLBACK_DICTDIR "radkfile");
}

void KDict::Destroy() {
	if(kdictSingleton) {
		delete kdictSingleton;
		kdictSingleton = NULL;
	}
}

int KDict::LoadKanjidic(const string& filename, const string& jisStd,
						const string& encoding) {
	int returnCode=KD_FAILURE;

	/* Get file names */
	string gzfn, fn;
	GetGzipName(filename, gzfn, fn);	

	/* Load file */
	string data;
	string *pFnOpened = NULL;
	if(ReadGzipIntoString(gzfn, data)) pFnOpened = &gzfn;
	else if(ReadFileIntoString(fn, data)) pFnOpened = &fn;
	if(!pFnOpened) return returnCode;

	/* Set the raw data pointer to point towards the a copy of the data
	   just read, or towards a converted version. */
	char* rawData = NULL;
	if(ToLower(encoding)!="utf-8") {
		/* Convert from source encoding to UTF-8 */
		string convertedData = ConvertString<char, char>(
			data, encoding.c_str(), "utf-8");
		/* If conversion fails, fall back and assume data is already in
		   UTF-8. */
		if(convertedData.empty()) convertedData = data;

		rawData = new char[convertedData.length()+1];
		strcpy(rawData, convertedData.c_str());
	} else {
		rawData = new char[data.length()+1];
		strcpy(rawData, data.c_str());
	}

	/* Create the kanjidic object with our string data. */
	this->KanjidicParser(rawData, jisStd.c_str());

	delete[] rawData;

	returnCode = KD_SUCCESS;
	el.Push(EL_Silent, string("Kanji dictionary file \"")
			.append(*pFnOpened).append("\" loaded successfully."));

	return returnCode;
}

/* Optimized based on occurance of fields in kanjidic2,
   db ver 2006-512, date 2008-02-12 */
int KDict::LoadKanjidic2(const string& filename) {
	int returnCode = KD_FAILURE;

	xmlTextReaderPtr reader;
	xmlChar* ptr;
	int ret;

	/* Vars for navigating through the data */
	string element, d1element;
	int nodeType;
	bool isAttribute=false;
	string attr_cp_type, attr_dr_type, attr_m_lang, attr_m_page, attr_m_vol,
		attr_qc_type, attr_rad_type, attr_r_type, attr_skip_misclass,
		attr_var_type, attr_on_type, attr_r_status;
	
	/* Var for storing values of the entries */
	string sValue;
	/* GP vars */
	string temp;

	/* Get file names */
	string gzfn, fn;
	GetGzipName(filename, gzfn, fn);	

	/* Load file */
	string data;
	string *pFnOpened = NULL;
	if(ReadGzipIntoString(gzfn, data)) pFnOpened = &gzfn;
	else if(ReadFileIntoString(fn, data)) pFnOpened = &fn;
	if(!pFnOpened) return returnCode;

	/* Get reader object */
	reader = xmlReaderForMemory(data.c_str(), data.length(), NULL, NULL, 0);

	KInfo* k=NULL;
	if(reader) {
		ret = xmlTextReaderRead(reader);
		while(ret==1) {
			/* Act based on node type */
			nodeType = xmlTextReaderNodeType(reader);
			switch(nodeType) {
			case XML_READER_TYPE_ELEMENT:
				element = (char*)xmlTextReaderName(reader);
				if(xmlTextReaderDepth(reader)==1) d1element=element;
				if(element=="character") {
					/* Opening of character entry - create new data object */
					if(k) delete k;
					k = new KInfo;
				}
				attr_cp_type = attr_dr_type = attr_m_lang = attr_m_page
					= attr_m_vol = attr_qc_type = attr_rad_type = attr_r_type
					= attr_skip_misclass = attr_var_type = "";
				break;
			case XML_READER_TYPE_END_ELEMENT:
				element = (char*)xmlTextReaderName(reader);
				if(element=="character") {
					wchar_t wc = utfconv_mw(k->literal)[0];
					/* End of character entry: append to data list */
					kdictData[wc] = *k;
					delete k;
					k = NULL;
				}
				attr_cp_type = attr_dr_type = attr_m_lang = attr_m_page
					= attr_m_vol = attr_qc_type = attr_rad_type = attr_r_type
					= attr_skip_misclass = attr_var_type = "";
				break;
			case XML_READER_TYPE_ATTRIBUTE:
				temp = (char*)xmlTextReaderName(reader);
				ptr = xmlTextReaderValue(reader);
				if(temp == "r_type") attr_r_type = (char*)ptr;
				else if(temp == "dr_type") attr_dr_type = (char*)ptr;
				else if(temp == "cp_type") attr_cp_type = (char*)ptr;
				else if(temp == "qc_type") attr_qc_type = (char*)ptr;
				else if(temp == "m_lang") attr_m_lang = (char*)ptr;
				else if(temp == "rad_type") attr_rad_type = (char*)ptr;
				else if(temp == "m_page") attr_m_page = (char*)ptr;
				else if(temp == "m_vol") attr_m_vol = (char*)ptr;
				else if(temp == "var_type") attr_var_type = (char*)ptr;
				else if(temp == "skip_misclass")
					attr_skip_misclass = (char*)ptr;
				/* These vars are currently unused by J-Ben, but they are
				   valid kanjidic2 fields */
				else if(temp == "on_type") attr_on_type = (char*)ptr;
				else if(temp == "r_status") attr_r_status = (char*)ptr;
				/* Bail if something unexpected shows */
				else assert(0 && "attribute error");
				xmlFree(ptr);
				break;
			case XML_READER_TYPE_TEXT:
				ptr = xmlTextReaderValue(reader);
				sValue = (char*)ptr;
				xmlFree(ptr);

				if(d1element=="header") {
					if(element=="file_version") {
						if(sValue!="4") {
							ostringstream oss;
							oss << ERR_PREF
								<< "Warning: the KANJIDIC2 reader only"
								" supports KANJIDIC2 version 4!";
							el.Push(EL_Warning, oss.str());
						}
					}
				}
				if(d1element=="character") {
					if(!k) {
						ostringstream oss;
						oss << ERR_PREF << "k is NULL!";
						el.Push(EL_Error, oss.str());
					} else if(element=="literal") {
						k->literal = sValue;
					} else if(element=="cp_value") {
						string &s = attr_cp_type;
						if(s == "jis208") k->cp_j208 = sValue;
						else if(s == "jis212") k->cp_j212 = sValue;
						else if(s == "jis213") k->cp_j213 = sValue;
						else if(s == "ucs") k->cp_ucs = sValue;
						else assert(0 && "Shouldn't happen!");
					} else if(element=="rad_value") {
						temp = attr_rad_type;
						if(temp == "classical")
							k->radical
								= (unsigned char)atoi(sValue.c_str());
						else if(temp == "nelson_c")
							k->radicalNelson
								= (unsigned char)atoi(sValue.c_str());
						else {
							ostringstream oss;
							oss << ERR_PREF
								<< "Unhandled radical: "
								<< "type=" << temp
								<< ", value=[" << sValue
								<< "]!";
							el.Push(EL_Error, oss.str());
						}
					} else if(element=="grade") {
						k->grade = (unsigned char)atoi(sValue.c_str());
					} else if(element=="stroke_count") {
						k->strokeCount = (unsigned char)atoi(sValue.c_str());
					} else if(element=="variant") {
						string& s = attr_var_type;
						if(s == "jis208") k->var_j208 = sValue;
						else if(s == "jis212") k->var_j212 = sValue;
						else if(s == "nelson_c") k->var_nelson_c = sValue;
						else if(s == "ucs") k->var_ucs = sValue;
						else if(s == "deroo") k->var_deroo = sValue;
						else if(s == "njecd") k->var_njecd = sValue;
						else if(s == "oneill") k->var_oneill = sValue;
						else if(s == "jis213") k->var_j213 = sValue;
						else if(s == "s_h") k->var_s_h = sValue;
						else assert(0 && "vartype error");
					} else if(element=="freq") {
						k->freq = atoi(sValue.c_str());
					} else if(element=="rad_name") {
						k->radicalName = sValue;
					} else if(element=="dic_ref") {
						string& s = attr_dr_type;
						if(s == "moro") {
							k->dc_moro = sValue;
							if(!attr_m_vol.empty()) {
								temp = "V";
								temp += attr_m_vol;
								temp += 'P';
								temp += attr_m_page;
								k->dc_moro.append(temp);
							}
						}
						else if(s == "nelson_n")
							k->dc_nelson_n = sValue;
						else if(s == "nelson_c")
							k->dc_nelson_c = sValue;
						else if(s == "heisig")
							k->dc_heisig = sValue;
						else if(s == "halpern_njecd")
							k->dc_halpern_njecd = sValue;
						else if(s == "oneill_names")
							k->dc_oneill_names = sValue;
						else if(s == "halpern_kkld")
							k->dc_halpern_kkld = sValue;
						else if(s == "sh_kk")
							k->dc_sh_kk = sValue;
						else if(s == "gakken")
							k->dc_gakken = sValue;
						else if(s == "oneill_kk")
							k->dc_oneill_kk = sValue;
						else if(s == "kanji_in_context")
							k->dc_kanji_in_context = sValue;
						else if(s == "henshall")
							k->dc_henshall = sValue;
						else if(s == "kodansha_compact")
							k->dc_kodansha_compact = sValue;
						else if(s == "jf_cards")
							k->dc_jf_cards = sValue;
						else if(s == "henshall3")
							k->dc_henshall3 = sValue;
						else if(s == "tutt_cards")
							k->dc_tutt_cards = sValue;
						else if(s == "sakade")
							k->dc_sakade = sValue;
						else if(s == "crowley")
							k->dc_crowley = sValue;
						else if(s == "busy_people")
							k->dc_busy_people = sValue;
						else assert(0 && "dict error");
					} else if(element=="q_code") {
						string& s = attr_qc_type;
						if(s == "skip"
						   && (!attr_skip_misclass.empty())) {
							k->skipMisclass.push_back(
								pair<string,SkipCode>(
									attr_skip_misclass,
									SkipCode(sValue)));
						} else {
							if(s=="skip")
								k->qc_skip = SkipCode(sValue);
							else if(s=="four_corner")
								k->qc_four_corner = sValue;
							else if(s=="sh_desc")
								k->qc_sh_desc = sValue;
							else if(s=="deroo")
								k->qc_deroo = sValue;
							else assert(0 && "qc code error");
						}
					} else if(element=="reading") {
						temp = attr_r_type;
						if(temp=="pinyin") {
							k->pinyin.push_back(sValue);
						} else if(temp=="korean_r") {
							k->korean_r.push_back(sValue);					
						} else if(temp=="korean_h") {
							k->korean_h.push_back(sValue);
						} else if(temp=="ja_on") {
							/* Need to handle r_status and on_type! */
							/* Need to convert xx.x to xx(x) notation. */
							k->onyomi.push_back(
								utfconv_wm(
									ConvertKanjidicEntry(
										utfconv_mw(
											sValue))));
						} else if(temp=="ja_kun") {
							/* Need to handle r_status! */
							/* Need to convert xx.x to xx(x) notation. */
							k->kunyomi.push_back(
								utfconv_wm(
									ConvertKanjidicEntry(
										utfconv_mw(
											sValue))));
						} else {
							ostringstream oss;
							oss << ERR_PREF << "Invalid r_type: " << temp;
							el.Push(EL_Error, oss.str());
						}
						/* This section is "to-do" */
					} else if(element=="meaning") {
						temp = attr_m_lang;
						if(temp.empty())
							k->meaning.push_back(sValue);
						else if(temp == "es")
							k->meaning_es.push_back(sValue);
						else if(temp == "pt")
							k->meaning_pt.push_back(sValue);
						else assert(0 && "unsupported language");
					} else if(element=="nanori") {
						k->nanori.push_back(
							utfconv_wm(
								ConvertKanjidicEntry(
									utfconv_mw(
										sValue))));
					} else {
						ostringstream oss;
						oss << ERR_PREF << "UNHANDLED element: " << element;
						el.Push(EL_Error, oss.str());
					}
				}
				break;
			default:
				/* do nothing */
				break;
			}

			/* If element has attributes, go to the next attribute if present.
			   Otherwise, go to the next element. */
			if(!isAttribute) ret = xmlTextReaderHasAttributes(reader);
			if(isAttribute || ret==1) {
				ret = xmlTextReaderMoveToNextAttribute(reader);
			}
			/* ret==-1 is an error */
			if(ret==-1) {
				ostringstream oss;
				oss << ERR_PREF
					<< "xmlTextReaderMoveToNextAttribute returned an error!";
				el.Push(EL_Error, oss.str());
			}
			/* If ret==1, an attribute was loaded.
			   If not, go to the next element. */
			if(ret==1) {
				isAttribute=true;
			} else {
				isAttribute=false;
				ret = xmlTextReaderRead(reader);
			}
		}
		xmlFreeTextReader(reader);
		if(ret!=0) {
			ostringstream oss;
			oss << ERR_PREF
				<< "Parsing error occurred in " << filename << ".";
			el.Push(EL_Error, oss.str());
		}

		returnCode = KD_SUCCESS;
		el.Push(EL_Silent, string("Kanji dictionary file \"")
				.append(*pFnOpened).append("\" loaded successfully."));
	} else return returnCode;

	if(k) {
		ostringstream oss;
		oss << ERR_PREF << ": k is not NULL!  This shouldn't happen!";
		el.Push(EL_Error, oss.str());
		delete k;
		k = NULL;
	}

	return returnCode;
}

int KDict::LoadKradfile(const string& filename, const string& encoding) {
	int returnCode = KD_FAILURE;

	/* Get file names */
	string gzfn, fn;
	GetGzipName(filename, gzfn, fn);	

	/* Load file */
	string rawDataStr, dataStr;
	string *pFnOpened = NULL;
	if(ReadGzipIntoString(gzfn, rawDataStr)) pFnOpened = &gzfn;
	else if(ReadFileIntoString(fn, rawDataStr)) pFnOpened = &fn;
	if(!pFnOpened) return returnCode;

	if(ToLower(encoding)!="utf-8")
		dataStr = ConvertString<char, char>(
			rawDataStr, encoding.c_str(), "utf-8");
	if(dataStr.empty()) dataStr = rawDataStr;

	list<wstring> data =
		StrTokenize<wchar_t>(utfconv_mw(dataStr), L"\n");
	while(!data.empty()) {
		wstring token = data.front();
		data.pop_front();
		if((!token.empty()) && token[0]!=L'#') {
			/* Get rid of the spaces in the string */
			token = TextReplace<wchar_t>(token, L" ", L"");
			kdictData[token[0]].kradData = token.substr(2);
		}
	}

	returnCode = KD_SUCCESS;
	el.Push(EL_Silent, string("Kanji dictionary file \"")
			.append(*pFnOpened).append("\" loaded successfully."));

	return returnCode;
}

int KDict::LoadRadkfile(const string& filename, const string& encoding) {
	int returnCode = KD_FAILURE;

	/* Get file names */
	string gzfn, fn;
	GetGzipName(filename, gzfn, fn);	

	/* Load file */
	string rawDataStr, dataStr;
	string *pFnOpened = NULL;
	if(ReadGzipIntoString(gzfn, rawDataStr)) pFnOpened = &gzfn;
	else if(ReadFileIntoString(fn, rawDataStr)) pFnOpened = &fn;
	if(!pFnOpened) return returnCode;

	if(ToLower(encoding)!="utf-8")
		dataStr = ConvertString<char, char>(
			rawDataStr, encoding.c_str(), "utf-8");
	if(dataStr.empty()) dataStr = rawDataStr;

	/* RADKFILE entries all start with $.
	   Split on $, and discard the first entry since it is the explanation
	   preceding the first entry. */
	list<wstring> data =
		StrTokenize<wchar_t>(utfconv_mw(dataStr), L"$");
	data.pop_front();

	while(!data.empty()) {
		wstring entry = data.front();
		data.pop_front();
		if((!entry.empty()) && entry[0]!=L'#') {
			/* RADKFILE-specific stuff here */
			list<wstring> entryData =
				StrTokenize<wchar_t>(entry, L"\n", false, 2);
			if(entryData.size()!=2) {
				ostringstream oss;
				oss << ERR_PREF
					<< "Error: entryData.size() == " << entryData.size()
					<< " for entry \"" << utfconv_wm(entry) << "!!";
				el.Push(EL_Error, oss.str());
			} else {
				wchar_t key;
				int strokeCount;
				wstring value;
				/* entryData.front() contains our key.
				   It's a space delimited string,
				   first token is our kanji, second is the stroke count.
				   A third token may be present, but is irrelevant. */
				list<wstring> keyData =
					StrTokenize<wchar_t>(entryData.front(), L" ");
				wistringstream wiss;
				wiss.str(keyData.front());
				wiss >> key;
				keyData.pop_front();
				wiss.str(keyData.front());
				wiss >> strokeCount;

				/* entryData.back() contains the characters our key
				   maps to. */
				/* Get rid of the spaces in the string */
				value = entryData.back();
				value = TextReplace<wchar_t>(value, L"\n", L"");
				value = TextReplace<wchar_t>(value, L" ", L"");				

				radkData[key] = value;
				radkDataStrokes[key] = strokeCount;
			}
		}
	}

	returnCode = KD_SUCCESS;
	el.Push(EL_Silent, string("Kanji dictionary file \"")
			.append(filename).append("\" loaded successfully."));

	return returnCode;
}

string JisHexToKuten(const string& jisHex) {
	int i;
	stringstream ss(jisHex);
	ss >> hex >> i >> dec;
	ss.clear();
	ss << (((i & 0xFF00) >> 8) - 0x20)
	   << '-' << ((i & 0xFF) - 0x20);
	return ss.str();
}

/* This function converts from KANJIDIC-style entries to internally used
   KInfo objects (which are structurally based off the newer KANJIDIC2). */
void KDict::KanjidicToKInfo(const string& kanjidicEntry,
							KInfo& k, const char* jisStd) {
	list<string> tl = StrTokenize<char>(kanjidicEntry, " ");
	if(tl.size()<2) return; /* KANJIDIC entries must AT LEAST have the char
							   and the JIS hex code. */
	int tmode = 0;
	string sTemp;
	wstring wsTemp;
	wchar_t cKanaTest;

	/* First 2 fields are always the same: process them here */
	k.literal = tl.front(); tl.pop_front();
	/* JIS code needs to be converted to ku-ten
	   format to coincide with KANJIDIC2. */
	if(strcmp(jisStd, "jis208")==0) {
		k.cp_j208 = JisHexToKuten(tl.front()); tl.pop_front();
	} else if(strcmp(jisStd, "jis212")==0) {
		k.cp_j212 = JisHexToKuten(tl.front()); tl.pop_front();
	} else if(strcmp(jisStd, "jis213")==0) {
		k.cp_j213 = JisHexToKuten(tl.front()); tl.pop_front();		
	} else assert(0 && "invalid jis page");

	/* Now, just loop through the remaining entries in the list. */
	string* ps;
	while(!tl.empty()) {
		ps = &(tl.front());
		switch ((*ps)[0]) {
		case 'T':  /* Change "t mode" */
			tmode = atoi(ps->substr(1).c_str());
			break;
		case 'B':  /* Nelson-reclassified radical */
			k.radicalNelson = (unsigned char)atoi(ps->substr(1).c_str());
			break;
		case 'C':  /* Classical radical (KangXi Zidian) */
			k.radical = (unsigned char)atoi(ps->substr(1).c_str());
			break;
		case 'F':  /* Frequency */
			k.freq = atoi(ps->substr(1).c_str());
			break;
		case 'G':  /* Grade level */
			k.grade = atoi(ps->substr(1).c_str());
			break;
		case 'S':  /* Stroke count */
			if(k.strokeCount==0)
				k.strokeCount = atoi(ps->substr(1).c_str());
			else
				k.misstrokes.push_back(atoi(ps->substr(1).c_str()));
			break;
		case 'U':  /* Unicode value */
			k.cp_ucs = ps->substr(1);
			break;
		/* Dictionary codes for most of the following */
		case 'H':
			/* New Japanese-English Character Dictionary (Halpern) */
			k.dc_halpern_njecd = ps->substr(1);
			break;
		case 'N':
			/* Modern Reader's Japanese-English Character Dictionary (Nelson) */
			k.dc_nelson_c = ps->substr(1);
			break;
		case 'V':
			/* The New Nelson's Japanese-English Character Dictionary */
			k.dc_nelson_n = ps->substr(1);
			break;
		case 'P':
			/* SKIP codes. */
			/* Thanks to changes in permissible SKIP code usage (change to
			   Creative Commons licensing in January 2008), we can now use
			   this without problems. */
			k.qc_skip = SkipCode(ps->substr(1));
			break;
		case 'I':  /* Spahn/Hadamitzky dictionaries */
			if((*ps)[1]=='N') {
				/* Kanji & Kana (Spahn, Hadamitzky) */
				k.dc_sh_kk = ps->substr(2);
			} else {
				/* Query Code: Kanji Dictionary (Spahn, Hadamitzky) */
				k.qc_sh_desc = ps->substr(1);
			}
			break;
		case 'Q':
			/* Four Corner code */
			k.qc_four_corner = ps->substr(1);
			break;
		case 'M':
			if((*ps)[1]=='N') {
				/* Morohashi Daikanwajiten Index */
				k.dc_moro.insert(0, ps->substr(2));
			} else if((*ps)[1]=='P') {
				/* Morohashi Daikanwajiten Volume/Page */
				k.dc_moro
					.append(1, '/')
					.append(ps->substr(2));
			}
			break;
		case 'E':
			/* A Guide to Remembering Japanese Characters (Henshall) */
			k.dc_henshall = ps->substr(1);
			break;
		case 'K':
			/* Gakken Kanji Dictionary ("A New Dictionary of Kanji Usage") */
			k.dc_gakken = ps->substr(1);
			break;
		case 'L':
			/* Remembering the Kanji (Heisig) */
			k.dc_heisig = ps->substr(1);
			break;
		case 'O':
			/* Japanese Names (O'Neill) */
			k.dc_oneill_names = ps->substr(1);
			break;
		case 'D':
			switch((*ps)[1]) {
			case 'B':
				/* Japanese for Busy People (AJLT) */
				k.dc_busy_people = ps->substr(2);
				break;
			case 'C':
				/* The Kanji Way to Japanese Language Power (Crowley) */
				k.dc_crowley = ps->substr(2);
				break;
			case 'F':
				/* Japanese Kanji Flashcards (White Rabbit Press) */
				k.dc_jf_cards = ps->substr(2);
				break;
			case 'G':
				/* Kodansha Compact Kanji Guide */
				k.dc_kodansha_compact = ps->substr(2);
				break;
			case 'H':
				/* A Guide To Reading and Writing Japanese (Henshall) */
				k.dc_henshall3 = ps->substr(2);
				break;
			case 'J':
				/* Kanji in Context (Nishiguchi and Kono) */
				k.dc_kanji_in_context = ps->substr(2);
				break;
			case 'K':
				/* Kodansha Kanji Learner's Dictionary (Halpern) */
				k.dc_halpern_kkld = ps->substr(2);
				break;
			case 'O':
				/* Essential Kanji (O'Neill) */
				k.dc_oneill_kk = ps->substr(2);
				break;
			case 'R':
				/* Query Code: 2001 Kanji (De Roo) */
				k.qc_deroo = ps->substr(2);
				break;
			case 'S':
				/* A Guide to Reading and Writing Japanese (Sakade) */
				k.dc_sakade = ps->substr(2);
				break;
			case 'T':
				/* Tuttle Kanji Cards (Kask) */
				k.dc_tutt_cards = ps->substr(2);
				break;
			default:
				{
					ostringstream oss;
					oss << ERR_PREF << "Unhandled: " << *ps;
					el.Push(EL_Error, oss.str());
				}
			break;
			}
			break;
			/* Crossreferences and miscodes */
		case 'X':
			switch((*ps)[1]) {
			case 'D':
				/* De Roo code */
				k.var_deroo=ps->substr(2);
				break;	
			case 'H':
				/* NJECD code */
				k.var_njecd=ps->substr(2);
				break;
			case 'I':
				/* S_H code */
				k.var_s_h=ps->substr(2);
				break;
			case 'J':
				/* XJ# = JIS hex code: 0=jis208, 1=jis212, 2=jis213 */
				switch((*ps)[2]) {
				case '0':
					k.var_j208=JisHexToKuten(ps->substr(3));
					break;
				case '1':
					k.var_j212=JisHexToKuten(ps->substr(3));
					break;
				case '2':
					k.var_j213=JisHexToKuten(ps->substr(3));
					break;
				}
				break;
			case 'N':
				/* nelson_c code */
				k.var_nelson_c=ps->substr(2);
				break;
			case 'O':
				/* oneill code */
				k.var_oneill=ps->substr(2);
				break;
			default:
				{
					ostringstream oss;
					oss << ERR_PREF << "Unknown entry \"" << *ps << "\" found!";
					el.Push(EL_Error, oss.str());
				}
			}
			break;
		case 'Z':
			sTemp = ps->substr(0,3);
			if(sTemp == "ZBP")
				k.skipMisclass.push_back(
					pair<string,SkipCode>("stroke_and_posn", SkipCode(ps->substr(3))));
			else if(sTemp == "ZPP") {
				k.skipMisclass.push_back(
					pair<string,SkipCode>("posn", SkipCode(ps->substr(3))));
			} else if(sTemp == "ZRP") {
				k.skipMisclass.push_back(
					pair<string,SkipCode>("stroke_diff", SkipCode(ps->substr(3))));
			} else if(sTemp == "ZSP") {
				k.skipMisclass.push_back(
					pair<string,SkipCode>("stroke_count", SkipCode(ps->substr(3))));
			} else {
				ostringstream oss;
				oss << ERR_PREF << "Unknown entry \"" << *ps << "\" found!";
				el.Push(EL_Error, oss.str());
			}
			break;
		/* Korean/Pinyin (Chinese) romanization */
		case 'W':
			k.korean_r.push_back(ps->substr(1));
			break;
		case 'Y':
			k.pinyin.push_back(ps->substr(1));
			break;
		case '{':
			/* MEANINGS */
			sTemp = *ps;
			/* Make sure we grab the whole meaning entry - pop more tokens and
			   append if necessary. */
			while(*(sTemp.rbegin()) != '}') {
				tl.pop_front();
				if(tl.empty()) break;
				sTemp.append(1, ' ');
				sTemp.append(tl.front());
			}
			if(*(sTemp.rbegin()) != '}') {
				/* Shouldn't happen, but I want to be safe. */
				ostringstream oss;
				oss << ERR_PREF << "Unable to find ending '}' character!\n"
					<< "Entry responsible: [" << kanjidicEntry << "]";
				el.Push(EL_Error, oss.str());
				/* Strip only the starting {, since } is not present. */
				sTemp = sTemp.substr(1, sTemp.length()-1);
			} else {
				/* Strip {} from around the string. */
				sTemp = sTemp.substr(1, sTemp.length()-2);				
			}
			k.meaning.push_back(sTemp);
			break;
		default:
			switch(tmode) {
			case 0:
				/* Check for readings */
				/* The first character may be 〜, but if so, it -will- be
				   followed by a kana character. */
				wsTemp = utfconv_mw(*ps);
				if(wsTemp[0]==L'〜')
					cKanaTest = wsTemp[1];
				else cKanaTest = wsTemp[0];

				if(IsHiragana(cKanaTest)) {
					k.kunyomi.push_back(*ps);
				} else if(IsKatakana(cKanaTest)) {
					k.onyomi.push_back(*ps);
				} else {
					ostringstream oss;
					oss << ERR_PREF
						<< "UNHANDLED entry \"" << *ps << "\" encountered!";
					el.Push(EL_Error, oss.str());
				}

				break;
			case 1:
				k.nanori.push_back(*ps);
				break;
			case 2:
				k.radicalName = *ps;
				break;
			default:
				{
					ostringstream oss;
					oss << ERR_PREF
						<< "Unknown tmode value (" << tmode << ") encountered!";
					el.Push(EL_Error, oss.str());
				}
			}
			
			break;			
		}
		tl.pop_front();
	}
}

/* This could be sped up: copy the first UTF-8 character into a string, then
   run a conversion on that.  Trivial though. */
void KDict::KanjidicParser(char* kanjidicRawData, const char* jisStd) {
	char* token = strtok(kanjidicRawData, "\n");
	wstring wToken;
	while(token) {
		if( (strlen(token)>0) && (token[0]!='#') ) {
			wToken = utfconv_mw(token);
			/* Convert token to proper format */
			wToken = ConvertKanjidicEntry(wToken);
			/* Get and fill the relevant entry. */
			KInfo& k = kdictData[wToken[0]];
			KanjidicToKInfo(utfconv_wm(wToken), k, jisStd);
		}
		token = strtok(NULL, "\n");
	}
}

KDict::~KDict() {
	/* Currently: nothing here. */
}

/*
 * Performs transformations on a KANJIDIC string for our internal usage.
 * Currently, this includes the following:
 * - Changing あ.いう notation to あ(いう), a la JWPce/JFC.
 * - Changing -あい notation to 〜あい, also a la JWPce/JFC.
 */
wstring KDict::ConvertKanjidicEntry(const wstring& s) {
	size_t index, lastIndex;
	wstring temp = s;

	/* First conversion: あ.いう to あ(いう) */
	index = temp.find(L'.', 0);
	while(index!=wstring::npos) {
		/* Proceed if the character preceding the "." is hiragana/katakana. */
		if(IsFurigana(temp[index-1])) {
			temp[index] = L'(';
			index = temp.find(L' ', index+1);
			if(index==wstring::npos) {
				temp.append(1, L')');
				break;
			} else
				temp.insert(index, 1, L')');
		}
		lastIndex = index;
		index = temp.find(L'.', lastIndex+1);
	}

	/* Second conversion: - to 〜, when a neighboring
	   character is hiragana/katakana */
	index = temp.find(L'-', 0);
	while(index!=wstring::npos) {
		/* Proceed if the character before or after
		   the "-" is hiragana/katakana. */
		if(IsFurigana(temp[index-1]) || IsFurigana(temp[index+1]))
			temp[index]=L'〜';

		lastIndex = index;
		index = temp.find(L'-', lastIndex+1);
	}

	/* Return the converted string */
	return temp;
}

string GetSODHtml(const KInfo& k, long options) {
	string utfStr;

	/* Get the UTF8-encoded string for the kanji. */
	utfStr = k.literal;
	/* Convert to a low-to-high-byte hex string. */
	ostringstream ss;
	for(unsigned int i=0;i<utfStr.length();i++) {
		ss << hex << setw(2) << setfill('0')
		   << (unsigned int)((unsigned char)utfStr[i]);
	}

	ostringstream filename, sod;
	Preferences* p = Preferences::Get();
	string sodDir = p->GetSetting("sod_dir");
	if(sodDir.empty()) sodDir = JB_DATADIR DSSTR "sods";

	/* Load static SOD, if present */
	if((options & KDO_SOD_STATIC) != 0) {
		filename << sodDir << DSCHAR
				 << "sod-utf8-hex" << DSCHAR
				 << ss.str() << ".png";
		ifstream f(filename.str().c_str());
		if(f.is_open()) {
			f.close();
			sod << "<img src=\"" << filename.str() << "\" />";
		}
	}
	/* Load animated SOD, if present */
	if((options & KDO_SOD_ANIM) != 0) {
		filename.clear();
		filename << sodDir << DSCHAR
				 << "soda-utf8-hex" << DSCHAR
				 << ss.str() << ".gif";
		ifstream f(filename.str().c_str());
		if(f.is_open()) {
			f.close();
			if(!(sod.str().empty())) sod << "<br />";
			sod << "<img src=\"" << filename.str() << "\" />";
		}
	}

	string result;
	if(!(sod.str().empty())) {
		/* Append the chart(s) in a paragraph object. */
		result.append("<p>");
		sod << "<br /><font size=\"1\">(Kanji stroke order graphics "
			"used under license from KanjiCafe.com.)</font></p>";
		result.append(sod.str());
	}
	return result;
}

wstring KDict::KInfoToHtml(const KInfo& k) {
	Preferences* prefs = Preferences::Get();
	return KInfoToHtml(k,
					   prefs->kanjidicOptions,
					   prefs->kanjidicDictionaries);
}

wstring KDict::KInfoToHtml(const KInfo& k,
						   long options, long dictionaries) {
	ostringstream result;
	list<string>::const_iterator lsi;
	list<int>::const_iterator lii;

	/* Create header: char, optional sod display */
	result << "<p><font size=\"7\">" << k.literal << "</font></p>";

	/* Insert KanjiCafe.com SODs if present */
	if((options & (KDO_SOD_STATIC | KDO_SOD_ANIM)) != 0) {
		result << GetSODHtml(k, options);
	}

	result << "<ul>";
	/* Japanese readings */
	if((options & KDO_READINGS) != 0) {
		if(!k.onyomi.empty()) {
			result << "<li>Onyomi Readings: ";
			lsi = k.onyomi.begin();
			result << *lsi;
			for(lsi++; lsi!=k.onyomi.end(); lsi++) {
				result << "&nbsp; " << *lsi;
			}
			result << "</li>";
		}
		if(!k.kunyomi.empty()) {
			result << "<li>Kunyomi Readings: ";
			lsi = k.kunyomi.begin();
			result << *lsi;
			for(lsi++; lsi!=k.kunyomi.end(); lsi++) {
				result << "&nbsp; " << *lsi;
			}
			result << "</li>";
		}
		if(!k.nanori.empty()) {
			result << "<li>Nanori Readings: ";
			lsi = k.nanori.begin();
			result << *lsi;
			for(lsi++; lsi!=k.nanori.end(); lsi++) {
				result << "&nbsp; " << *lsi;
			}
			result << "</li>";
		}
		if(!k.radicalName.empty()) {
			result << "<li>Special Radical Reading: "
				   << k.radicalName << "</li>";
		}
	}

	if((options & KDO_MEANINGS) != 0) {
		/* Meanings (English) */
		if(!k.meaning.empty()) {
			result << "<li>English Meanings: ";
			lsi = k.meaning.begin();
			result << *lsi;
			for(lsi++; lsi!=k.meaning.end(); lsi++) {
				result << "; " << *lsi;
			}
			result << "</li>";
		}
		/* Meanings (Other languages) */
		/* -- NOT YET IMPLEMENTED -- */
	}

	/* Stroke count (and miscounts), grade, freq */
	if((options & KDO_HIGHIMPORTANCE) != 0) {
		if(k.strokeCount>0) {
			result << "<li>Stroke count: " << k.strokeCount;
			if(!k.misstrokes.empty()) {
				result << " (commonly miscounted as ";
				lii = k.misstrokes.begin();
				result << *lii;
				for(lii++; lii!=k.misstrokes.end(); lii++) {
					result << ", " << *lii;
				}
				result << ')';
			}
			result << "</li>";
		}
		else result << "<li>Stroke count: unspecified</li>";
		result << "<li>Grade level: ";
		if(k.grade <= 6 && k.grade >= 1)
			result << k.grade;
		else if(k.grade == 8)
			result << "General usage";
		else if(k.grade == 9)
			result << "Jinmeiyou (Characters for names)";
		else if(k.grade == 0)
			result << "Unspecified";
		else
			result << "Unhandled grade level (Grade " << k.grade << ')';
		result << "</li>";
		if(k.freq==0)
			result << "<li>Frequency ranking: Unspecified</li>";
		else
			result << "<li>Frequency ranking: " << k.freq << "</li>";
	}

	if((options & KDO_MULTIRAD) != 0) {
		if(!k.kradData.empty()) {
			result << "<li>Component radicals: "
				   << utfconv_wm(k.kradData) << "</li>";
		}
	}

	/* Vocab List Cross-ref */
	if((options & KDO_VOCABCROSSREF) != 0) {
		ListManager* lm = ListManager::Get();
		vector<wstring> *vList = &(lm->VList()->GetVocabList());
		wchar_t thisKanji = utfconv_mw(k.literal)[0];
		vector<wstring> crossRefList;
		vector<wstring>::iterator vIt;
		for(vIt=vList->begin(); vIt!=vList->end(); vIt++) {
			if(vIt->find(thisKanji)!=wstring::npos) {
				crossRefList.push_back(*vIt);
			}
		}
		if(!crossRefList.empty()) {
			result << "<li>This kanji is used by words in your "
				"study list:<br><font size=\"7\">";
			vIt = crossRefList.begin();
			result << utfconv_wm(*vIt);
			for(++vIt; vIt!=crossRefList.end(); vIt++) {
				result << "&nbsp; " << utfconv_wm(*vIt);
			}
			result << "</font></li>";
		}
	}

	if((options & KDO_DICTIONARIES) != 0) {
		ostringstream dictOut;
		/* Dict/Query codes - ADD SKIP mis-codes! */
		/* Show Query codes first, followed by dict codes */
		if(!k.qc_deroo.empty()) {
			if((dictionaries & KDD_DR) != 0)
				dictOut << "<li>De Roo code: "
						<< k.qc_deroo << "</li>";
		}
		if(!k.qc_four_corner.empty()) {
			if((dictionaries & KDD_FC) != 0)
				dictOut << "<li>Four Corner code: "
						<< k.qc_four_corner << "</li>";
		}
		if(!k.qc_sh_desc.empty()) {
			if((dictionaries & KDD_KD) != 0)
				dictOut << "<li>Spahn/Hadamitzky Kanji Dictionary code: "
						<< k.qc_sh_desc << "</li>";
		}
		if(k.qc_skip.is_set()) {
			if((dictionaries & KDD_SKIP) != 0)
			dictOut << "<li>SKIP code: " << k.qc_skip.str();
			if(!k.skipMisclass.empty()) {
				/* Display miscode info */
				list< pair<string, SkipCode> >::const_iterator iMiscode;
				for(iMiscode = k.skipMisclass.begin();
					iMiscode != k.skipMisclass.end(); iMiscode++) {
					dictOut << "<br />Miscode (" << iMiscode->first
							<< "): " << iMiscode->second.str();
				}
			}
			dictOut << "</li>";
		}
		if(!k.dc_busy_people.empty()) {
			if((dictionaries & KDD_JBP) != 0)
				dictOut << "<li>Japanese For Busy People (AJLT): "
						<< k.dc_busy_people << "</li>";		
		}
		if(!k.dc_crowley.empty()) {
			if((dictionaries & KDD_KWJLP) != 0)
				dictOut << "<li>The Kanji Way to Japanese Language Power "
					"(Crowley): " << k.dc_crowley << "</li>";		
		}
		if(!k.dc_gakken.empty()) {
			if((dictionaries & KDD_GKD) != 0)
				dictOut << "<li>A New Dictionary of Kanji Usage (Gakken): "
						<< k.dc_gakken << "</li>";		
		}
		if(!k.dc_halpern_kkld.empty()) {
			if((dictionaries & KDD_KLD) != 0)
				dictOut << "<li>Kodansha Kanji Learners Dictionary (Halpern): "
						<< k.dc_halpern_kkld << "</li>";		
		}
		if(!k.dc_halpern_njecd.empty()) {
			if((dictionaries & KDD_NJECD) != 0)
				dictOut << "<li>New Japanese-English Character Dictionary "
					"(Halpern): " << k.dc_halpern_njecd << "</li>";		
		}
		if(!k.dc_heisig.empty()) {
			if((dictionaries & KDD_RTK) != 0)
				dictOut << "<li>Remembering the Kanji (Heisig): "
						<< k.dc_heisig << "</li>";		
		}
		if(!k.dc_henshall.empty()) {
			if((dictionaries & KDD_GRJC) != 0)
				dictOut << "<li>A Guide To Remembering Japanese Characters "
					"(Henshall): " << k.dc_henshall << "</li>";		
		}
		if(!k.dc_henshall3.empty()) {
			if((dictionaries & KDD_GTRWJH) != 0)
				dictOut << "<li>A Guide To Reading and Writing Japanese "
					"(Henshall): " << k.dc_henshall3 << "</li>";		
		}
		if(!k.dc_jf_cards.empty()) {
			if((dictionaries & KDD_JKF) != 0)
				dictOut << "<li>Japanese Kanji Flashcards (Hodges/Okazaki): "
						<< k.dc_jf_cards << "</li>";		
		}
		if(!k.dc_kanji_in_context.empty()) {
			if((dictionaries & KDD_KIC) != 0)
				dictOut << "<li>Kanji in Context (Nishiguchi/Kono): "
						<< k.dc_kanji_in_context << "</li>";		
		}
		if(!k.dc_kodansha_compact.empty()) {
			if((dictionaries & KDD_KCKG) != 0)
				dictOut << "<li>Kodansha Compact Kanji Guide: "
						<< k.dc_kodansha_compact << "</li>";		
		}
		if(!k.dc_moro.empty()) {
			if((dictionaries & KDD_MORO) != 0)
				dictOut << "<li>Morohashi Daikanwajiten: "
						<< k.dc_moro << "</li>";		
		}
		if(!k.dc_nelson_c.empty()) {
			if((dictionaries & KDD_MRJECD) != 0)
				dictOut << "<li>Modern Reader's Japanese-English Character "
					"Dictionary (Nelson): "
						<< k.dc_nelson_c << "</li>";		
		}
		if(!k.dc_nelson_n.empty()) {
			if((dictionaries & KDD_NNJECD) != 0)
				dictOut << "<li>The New Nelson Japanese-English Character "
					"Dictionary (Haig): " << k.dc_nelson_n << "</li>";		
		}
		if(!k.dc_oneill_kk.empty()) {
			if((dictionaries & KDD_EK) != 0)
				dictOut << "<li>Essential Kanji (O'Neill): "
						<< k.dc_oneill_kk << "</li>";		
		}
		if(!k.dc_oneill_names.empty()) {
			if((dictionaries & KDD_JN) != 0)
				dictOut << "<li>Japanese Names (O'Neill): "
						<< k.dc_oneill_names << "</li>";		
		}
		if(!k.dc_sakade.empty()) {
			if((dictionaries & KDD_GTRWJS) != 0)
				dictOut << "<li>A Guide To Reading and Writing Japanese "
					"(Sakade): " << k.dc_sakade << "</li>";		
		}
		if(!k.dc_sh_kk.empty()) {
			if((dictionaries & KDD_KK) != 0)
				dictOut << "<li>Kanji and Kana (Spahn/Hadamitzky): "
						<< k.dc_sh_kk << "</li>";		
		}
		if(!k.dc_tutt_cards.empty()) {
			if((dictionaries & KDD_TKC) != 0)
				dictOut << "<li>Tuttle Kanji Cards (Kask): "
						<< k.dc_tutt_cards << "</li>";		
		}
		/* Append dict str to result */
		if(!(dictOut.str().empty()))
			result << "<li>Dictionary Codes:<ul>"
				   << dictOut.str() << "</ul></li>";
		else result << "<li>No dictionary codes found.</li>";
	}

	/* Low importance stuff */
	if((options & KDO_LOWIMPORTANCE) != 0) {
		result << "<li>Extra Information:<ul>";
		/* JIS codes, UTF-8 codes */
		result << "<li>Character codes:<ul>";
		if(!k.cp_j208.empty())
			result << "<li>"
				   << "JIS-208: " << k.cp_j208 << "</li>";
		if(!k.cp_j212.empty())
			result << "<li>"
				   << "JIS-212: " << k.cp_j212 << "</li>";
		if(!k.cp_j213.empty())
			result << "<li>"
				   << "JIS-213: " << k.cp_j213 << "</li>";
		if(!k.cp_ucs.empty())
			result << "<li>"
				   << "Unicode: " << k.cp_ucs << "</li>";
		result << "</ul></li>";
		/* Classical/Nelson radicals */
		if(k.radical!=0)
			result << "<li>KangXi Zidian radical: "
				   << (unsigned int)k.radical << "</li>";
		if(k.radicalNelson!=0)
			result << "<li>Nelson radical: "
				   << (unsigned int)k.radicalNelson << "</li>";
		/* Pinyin/Korean */
		if(!k.pinyin.empty()) {
			result << "<li>Pinyin romanization: ";
			lsi = k.pinyin.begin();
			result << *lsi;
			for(lsi++; lsi!=k.pinyin.end(); lsi++) {
				result << ", " << *lsi;
			}
			result << "</li>";
		}
		if(!k.korean_h.empty()) {
			result << "<li>Korean reading: ";
			lsi = k.korean_h.begin();
			result << *lsi;
			for(lsi++; lsi!=k.korean_h.end(); lsi++) {
				result << "&nbsp; " << *lsi;
			}
			result << "</li>";
		}
		if(!k.korean_r.empty()) {
			result << "<li>Korean romanization: ";
			lsi = k.korean_r.begin();
			result << *lsi;
			for(lsi++; lsi!=k.korean_r.end(); lsi++) {
				result << ", " << *lsi;
			}
			result << "</li>";
		}
		/* Crossref codes */
		result << "<li>Cross References:<ul>";
		if(!k.var_j208.empty())
			result << "<li>JIS-208: " << k.var_j208 << "</li>";
		if(!k.var_j212.empty())
			result << "<li>JIS-212: " << k.var_j212 << "</li>";
		if(!k.var_j213.empty())
			result << "<li>JIS-213: " << k.var_j213 << "</li>";
		if(!k.var_ucs.empty())
			result << "<li>Unicode: " << k.var_ucs << "</li>";
		if(!k.var_deroo.empty())
			result << "<li>De Roo code: " << k.var_deroo << "</li>";
		if(!k.var_nelson_c.empty())
			result << "<li>Modern Reader's Japanese-English Character "
				"Dictionary (Nelson): " << k.var_nelson_c << "</li>";
		if(!k.var_njecd.empty())
			result << "<li>New Japanese-English Character Dictionary "
				"(Halpern): " << k.var_njecd << "</li>";
		if(!k.var_oneill.empty())
			result << "<li>Japanese Names (O'Neill): " << k.var_oneill
				   << "</li>";
		if(!k.var_s_h.empty())
			result << "<li>Spahn/Hadamitzky Kanji Dictionary code: "
				   << k.var_s_h << "</li>";
		result << "</ul></li>";
		
		/* TO DO */
		result << "</ul></li>";
	}

	result << "</ul>";
	return utfconv_mw(result.str());
}

string GetSODTextBuf(const KInfo& k, long options) {
	string utfStr;

	/* Get the UTF8-encoded string for the kanji. */
	utfStr = k.literal;
	/* Convert to a low-to-high-byte hex string. */
	ostringstream ss;
	for(unsigned int i=0;i<utfStr.length();i++) {
		ss << hex << setw(2) << setfill('0')
		   << (unsigned int)((unsigned char)utfStr[i]);
	}

	ostringstream filename, sod;
	Preferences* p = Preferences::Get();
	string sodDir = p->GetSetting("sod_dir");
	if(sodDir.empty()) sodDir = "sods";

	/* Load static SOD, if present */
	if((options & KDO_SOD_STATIC) != 0) {
		filename << sodDir << DSCHAR
				 << "sod-utf8-hex" << DSCHAR
				 << ss.str() << ".png";
		ifstream f(filename.str().c_str());
		if(f.is_open()) {
			f.close();
			sod << "<img " << filename.str() << '>';
		}
	}
	/* Load animated SOD, if present */
	if((options & KDO_SOD_ANIM) != 0) {
		filename.clear();
		filename << sodDir << DSCHAR
				 << "soda-utf8-hex" << DSCHAR
				 << ss.str() << ".gif";
		ifstream f(filename.str().c_str());
		if(f.is_open()) {
			f.close();
			if(!(sod.str().empty())) sod << "<br />";
			sod << "<img " << filename.str() << '>';
		}
	}

	string result;
	if(!(sod.str().empty())) {
		/* Append the chart(s) in a paragraph object. */
		sod << "\n<font en.small>(Kanji stroke order graphics "
			"used under license from KanjiCafe.com.)</font>\n\n";
		result.append(sod.str());
	}
	return result;
}

string KDict::KInfoToTextBuf(const KInfo& k) {
	Preferences* prefs = Preferences::Get();
	return KInfoToTextBuf(k,
						  prefs->kanjidicOptions,
						  prefs->kanjidicDictionaries);
}

string KDict::KInfoToTextBuf(const KInfo& k,
							 long options, long dictionaries) {
	ostringstream result;
	list<string>::const_iterator lsi;
	list<int>::const_iterator lii;

	/* Create header: char, optional sod display */
	result << "<font ja.large>" << k.literal << "</font>\n\n";

	/* Insert KanjiCafe.com SODs if present */
	if((options & (KDO_SOD_STATIC | KDO_SOD_ANIM)) != 0) {
		result << GetSODTextBuf(k, options);
	}

	/* Japanese readings */
	if((options & KDO_READINGS) != 0) {
		if(!k.onyomi.empty()) {
			result << "• Onyomi Readings: <font ja>";
			lsi = k.onyomi.begin();
			result << *lsi;
			for(lsi++; lsi!=k.onyomi.end(); lsi++) {
				result << "  " << *lsi;
			}
			result << "</font>\n";
		}
		if(!k.kunyomi.empty()) {
			result << "• Kunyomi Readings: <font ja>";
			lsi = k.kunyomi.begin();
			result << *lsi;
			for(lsi++; lsi!=k.kunyomi.end(); lsi++) {
				result << "  " << *lsi;
			}
			result << "</font>\n";
		}
		if(!k.nanori.empty()) {
			result << "• Nanori Readings: <font ja>";
			lsi = k.nanori.begin();
			result << *lsi;
			for(lsi++; lsi!=k.nanori.end(); lsi++) {
				result << "  " << *lsi;
			}
			result << "</font>\n";
		}
		if(!k.radicalName.empty()) {
			result << "• Special Radical Reading: <font ja>"
				   << k.radicalName << "</font>\n";
		}
	}

	if((options & KDO_MEANINGS) != 0) {
		/* Meanings (English) */
		if(!k.meaning.empty()) {
			result << "• English Meanings: ";
			lsi = k.meaning.begin();
			result << *lsi;
			for(lsi++; lsi!=k.meaning.end(); lsi++) {
				result << "; " << *lsi;
			}
			result << '\n';
		}
		/* Meanings (Other languages) */
		/* -- NOT YET IMPLEMENTED -- */
	}

	/* Stroke count (and miscounts), grade, freq */
	if((options & KDO_HIGHIMPORTANCE) != 0) {
		if(k.strokeCount>0) {
			result << "• Stroke count: " << k.strokeCount;
			if(!k.misstrokes.empty()) {
				result << " (commonly miscounted as ";
				lii = k.misstrokes.begin();
				result << *lii;
				for(lii++; lii!=k.misstrokes.end(); lii++) {
					result << ", " << *lii;
				}
				result << ')';
			}
			result << '\n';
		}
		else result << "• Stroke count: unspecified\n";
		result << "• Grade level: ";
		if(k.grade <= 6 && k.grade >= 1)
			result << k.grade;
		else if(k.grade == 8)
			result << "General usage";
		else if(k.grade == 9)
			result << "Jinmeiyou (Characters for names)";
		else if(k.grade == 0)
			result << "Unspecified";
		else
			result << "Unhandled grade level (Grade " << k.grade << ')';
		result << '\n';
		if(k.freq==0)
			result << "• Frequency ranking: Unspecified\n";
		else
			result << "• Frequency ranking: " << k.freq << '\n';
	}

	if((options & KDO_MULTIRAD) != 0) {
		if(!k.kradData.empty()) {
			result << "• Component radicals: <font ja>"
				   << utfconv_wm(k.kradData) << "</font>\n";
		}
	}

	/* Vocab List Cross-ref */
	if((options & KDO_VOCABCROSSREF) != 0) {
		ListManager* lm = ListManager::Get();
		vector<wstring> *vList = &(lm->VList()->GetVocabList());
		wchar_t thisKanji = utfconv_mw(k.literal)[0];
		vector<wstring> crossRefList;
		vector<wstring>::iterator vIt;
		for(vIt=vList->begin(); vIt!=vList->end(); vIt++) {
			if(vIt->find(thisKanji)!=wstring::npos) {
				crossRefList.push_back(*vIt);
			}
		}
		if(!crossRefList.empty()) {
			result << "• This kanji is used by words in your "
				"study list:\n<font ja>";
			vIt = crossRefList.begin();
			result << utfconv_wm(*vIt);
			for(++vIt; vIt!=crossRefList.end(); vIt++) {
				result << "  " << utfconv_wm(*vIt);
			}
			result << "</font>\n";
		}
	}

	if((options & KDO_DICTIONARIES) != 0) {
		ostringstream dictOut;
		/* Dict/Query codes - ADD SKIP mis-codes! */
		/* Show Query codes first, followed by dict codes */
		if(!k.qc_deroo.empty()) {
			if((dictionaries & KDD_DR) != 0)
				dictOut << "\t• De Roo code: "
						<< k.qc_deroo << "</li>";
		}
		if(!k.qc_four_corner.empty()) {
			if((dictionaries & KDD_FC) != 0)
				dictOut << "\t• Four Corner code: "
						<< k.qc_four_corner << "</li>";
		}
		if(!k.qc_sh_desc.empty()) {
			if((dictionaries & KDD_KD) != 0)
				dictOut << "\t• Spahn/Hadamitzky Kanji Dictionary code: "
						<< k.qc_sh_desc << "</li>";
		}
		if(k.qc_skip.is_set()) {
			if((dictionaries & KDD_SKIP) != 0)
			dictOut << "\t• SKIP code: " << k.qc_skip.str();
			if(!k.skipMisclass.empty()) {
				/* Display miscode info */
				list< pair<string,SkipCode> >::const_iterator iMiscode;
				for(iMiscode = k.skipMisclass.begin();
					iMiscode != k.skipMisclass.end(); iMiscode++) {
					dictOut << "\n  Miscode (" << iMiscode->first
							<< "): " << iMiscode->second.str();
				}
			}
			dictOut << '\n';
		}
		if(!k.dc_busy_people.empty()) {
			if((dictionaries & KDD_JBP) != 0)
				dictOut << "\t• Japanese For Busy People (AJLT): "
						<< k.dc_busy_people << '\n';		
		}
		if(!k.dc_crowley.empty()) {
			if((dictionaries & KDD_KWJLP) != 0)
				dictOut << "\t• The Kanji Way to Japanese Language Power "
					"(Crowley): " << k.dc_crowley << '\n';		
		}
		if(!k.dc_gakken.empty()) {
			if((dictionaries & KDD_GKD) != 0)
				dictOut << "\t• A New Dictionary of Kanji Usage (Gakken): "
						<< k.dc_gakken << '\n';		
		}
		if(!k.dc_halpern_kkld.empty()) {
			if((dictionaries & KDD_KLD) != 0)
				dictOut << "\t• Kodansha Kanji Learners Dictionary (Halpern): "
						<< k.dc_halpern_kkld << '\n';		
		}
		if(!k.dc_halpern_njecd.empty()) {
			if((dictionaries & KDD_NJECD) != 0)
				dictOut << "\t• New Japanese-English Character Dictionary "
					"(Halpern): " << k.dc_halpern_njecd << '\n';		
		}
		if(!k.dc_heisig.empty()) {
			if((dictionaries & KDD_RTK) != 0)
				dictOut << "\t• Remembering the Kanji (Heisig): "
						<< k.dc_heisig << '\n';		
		}
		if(!k.dc_henshall.empty()) {
			if((dictionaries & KDD_GRJC) != 0)
				dictOut << "\t• A Guide To Remembering Japanese Characters "
					"(Henshall): " << k.dc_henshall << '\n';		
		}
		if(!k.dc_henshall3.empty()) {
			if((dictionaries & KDD_GTRWJH) != 0)
				dictOut << "\t• A Guide To Reading and Writing Japanese "
					"(Henshall): " << k.dc_henshall3 << '\n';		
		}
		if(!k.dc_jf_cards.empty()) {
			if((dictionaries & KDD_JKF) != 0)
				dictOut << "\t• Japanese Kanji Flashcards (Hodges/Okazaki): "
						<< k.dc_jf_cards << '\n';		
		}
		if(!k.dc_kanji_in_context.empty()) {
			if((dictionaries & KDD_KIC) != 0)
				dictOut << "\t• Kanji in Context (Nishiguchi/Kono): "
						<< k.dc_kanji_in_context << '\n';		
		}
		if(!k.dc_kodansha_compact.empty()) {
			if((dictionaries & KDD_KCKG) != 0)
				dictOut << "\t• Kodansha Compact Kanji Guide: "
						<< k.dc_kodansha_compact << '\n';		
		}
		if(!k.dc_moro.empty()) {
			if((dictionaries & KDD_MORO) != 0)
				dictOut << "\t• Morohashi Daikanwajiten: "
						<< k.dc_moro << '\n';		
		}
		if(!k.dc_nelson_c.empty()) {
			if((dictionaries & KDD_MRJECD) != 0)
				dictOut << "\t• Modern Reader's Japanese-English Character "
					"Dictionary (Nelson): "
						<< k.dc_nelson_c << '\n';		
		}
		if(!k.dc_nelson_n.empty()) {
			if((dictionaries & KDD_NNJECD) != 0)
				dictOut << "\t• The New Nelson Japanese-English Character "
					"Dictionary (Haig): " << k.dc_nelson_n << '\n';		
		}
		if(!k.dc_oneill_kk.empty()) {
			if((dictionaries & KDD_EK) != 0)
				dictOut << "\t• Essential Kanji (O'Neill): "
						<< k.dc_oneill_kk << '\n';		
		}
		if(!k.dc_oneill_names.empty()) {
			if((dictionaries & KDD_JN) != 0)
				dictOut << "\t• Japanese Names (O'Neill): "
						<< k.dc_oneill_names << '\n';		
		}
		if(!k.dc_sakade.empty()) {
			if((dictionaries & KDD_GTRWJS) != 0)
				dictOut << "\t• A Guide To Reading and Writing Japanese "
					"(Sakade): " << k.dc_sakade << '\n';		
		}
		if(!k.dc_sh_kk.empty()) {
			if((dictionaries & KDD_KK) != 0)
				dictOut << "\t• Kanji and Kana (Spahn/Hadamitzky): "
						<< k.dc_sh_kk << '\n';		
		}
		if(!k.dc_tutt_cards.empty()) {
			if((dictionaries & KDD_TKC) != 0)
				dictOut << "\t• Tuttle Kanji Cards (Kask): "
						<< k.dc_tutt_cards << '\n';		
		}
		/* Append dict str to result */
		if(!(dictOut.str().empty()))
			result << "• Dictionary Codes:\n"
				   << dictOut.str();
		else result << "• No dictionary codes found.\n";
	}

	/* Low importance stuff */
	if((options & KDO_LOWIMPORTANCE) != 0) {
		result << "• Extra Information:\n";
		/* JIS codes, UTF-8 codes */
		result << "\t• Character codes:\n";
		if(!k.cp_j208.empty())
			result << "\t\t• "
				   << "JIS-208: " << k.cp_j208 << '\n';
		if(!k.cp_j212.empty())
			result << "\t\t• "
				   << "JIS-212: " << k.cp_j212 << '\n';
		if(!k.cp_j213.empty())
			result << "\t\t• "
				   << "JIS-213: " << k.cp_j213 << '\n';
		if(!k.cp_ucs.empty())
			result << "\t\t• "
				   << "Unicode: " << k.cp_ucs << '\n';
		/* Classical/Nelson radicals */
		if(k.radical!=0)
			result << "\t• KangXi Zidian radical: "
				   << (unsigned int)k.radical << '\n';
		if(k.radicalNelson!=0)
			result << "\t• Nelson radical: "
				   << (unsigned int)k.radicalNelson << '\n';
		/* Pinyin/Korean */
		if(!k.pinyin.empty()) {
			result << "\t• Pinyin romanization: ";
			lsi = k.pinyin.begin();
			result << *lsi;
			for(lsi++; lsi!=k.pinyin.end(); lsi++) {
				result << ", " << *lsi;
			}
			result << '\n';
		}
		if(!k.korean_h.empty()) {
			result << "\t• Korean reading: ";
			lsi = k.korean_h.begin();
			result << *lsi;
			for(lsi++; lsi!=k.korean_h.end(); lsi++) {
				result << "  " << *lsi;
			}
			result << '\n';
		}
		if(!k.korean_r.empty()) {
			result << "\t• Korean romanization: ";
			lsi = k.korean_r.begin();
			result << *lsi;
			for(lsi++; lsi!=k.korean_r.end(); lsi++) {
				result << ", " << *lsi;
			}
			result << '\n';
		}
		/* Crossref codes */
		result << "\t• Cross References:\n";
		if(!k.var_j208.empty())
			result << "\t\t• JIS-208: " << k.var_j208 << '\n';
		if(!k.var_j212.empty())
			result << "\t\t• JIS-212: " << k.var_j212 << '\n';
		if(!k.var_j213.empty())
			result << "\t\t• JIS-213: " << k.var_j213 << '\n';
		if(!k.var_ucs.empty())
			result << "\t\t• Unicode: " << k.var_ucs << '\n';
		if(!k.var_deroo.empty())
			result << "\t\t• De Roo code: " << k.var_deroo << '\n';
		if(!k.var_nelson_c.empty())
			result << "\t\t• Modern Reader's Japanese-English Character "
				"Dictionary (Nelson): " << k.var_nelson_c << '\n';
		if(!k.var_njecd.empty())
			result << "\t\t• New Japanese-English Character Dictionary "
				"(Halpern): " << k.var_njecd << '\n';
		if(!k.var_oneill.empty())
			result << "\t\t• Japanese Names (O'Neill): " << k.var_oneill
				   << '\n';
		if(!k.var_s_h.empty())
			result << "\t\t• Spahn/Hadamitzky Kanji Dictionary code: "
				   << k.var_s_h << '\n';
	}

	return result.str();
}

const unordered_map<wchar_t,KInfo>* KDict::GetHashTable() const {
	return &kdictData;
}

bool KDict::MainDataLoaded() const {
	if(!kdictData.empty()) return true;
	return false;
}

const KInfo* KDict::GetEntry(const wchar_t key) const {
	unordered_map<wchar_t, KInfo>::const_iterator kci = kdictData.find(key);
	if(kci != kdictData.end())
		return &(kci->second);
	return NULL;;
}
