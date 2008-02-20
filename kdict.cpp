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
#include "encoding_convert.h"
#include "string_utils.h"
#include "file_utils.h"
#include "jutils.h"
#include "errorlog.h"
#include <libxml/xmlreader.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
using namespace std;

#ifdef __WXMSW__
#	define FALLBACK_DICTDIR "dicts\\"
#else
#	define FALLBACK_DICTDIR "dicts/"
#endif

KDict* KDict::kdictSingleton = NULL;

KInfo::KInfo() {
	radical = radicalNelson = (unsigned char) 0;
	grade = strokeCount = freq = 0;
}

const KDict* KDict::Get() {
	if(!kdictSingleton)
		kdictSingleton = new KDict;
	return kdictSingleton;
}

KDict::KDict() {
	Preferences* p = Preferences::Get();
	int result;
	/* Load KANJIDIC2, if present. */
	result = LoadKanjidic2(p->GetSetting("kdict_kanjidic2").c_str());
	if(result!=KD_SUCCESS)
		result = LoadKanjidic2(FALLBACK_DICTDIR "kanjidic2.xml");

	/* If KANJIDIC2 is not present, load KANJIDIC and/or KANJD212 */
	if(result!=KD_SUCCESS) {
		result = LoadKanjidic(p->GetSetting("kdict_kanjidic").c_str());
		if(result!=KD_SUCCESS) LoadKanjidic(FALLBACK_DICTDIR "kanjidic");
		result =
			LoadKanjidic(p->GetSetting("kdict_kanjd212").c_str(), "jis212");
		if(result!=KD_SUCCESS)
			LoadKanjidic(FALLBACK_DICTDIR "kanjd212", "jis212");
	}

	/* Load supplemental dictionary files */
	result = LoadKradfile(p->GetSetting("kdict_kradfile").c_str());
	if(result!=KD_SUCCESS) LoadKradfile(FALLBACK_DICTDIR "kradfile");
	result = LoadRadkfile(p->GetSetting("kdict_radkfile").c_str());
	if(result!=KD_SUCCESS) LoadRadkfile(FALLBACK_DICTDIR "radkfile");
}

void KDict::Destroy() {
	if(kdictSingleton) {
		delete kdictSingleton;
		kdictSingleton = NULL;
	}
}

int KDict::LoadKanjidic(const char* filename, const char* jisStd) {
	char* rawData = NULL;
	unsigned int size;
	int returnCode=KD_FAILURE;

	ifstream ifile(filename, ios::ate); /* "at end" to get our file size */
	if(ifile) {
		size = ifile.tellg();
		ifile.seekg(0);
		rawData = new char[size+1];
		rawData[size] = '\0';
		ifile.read(rawData, size);
		if(strlen(rawData)!=size) {
			ostringstream oss;
			oss << ERR_PREF
				<< "kanjidic file size: "
				<< strlen(rawData)
				<< ", read-in string: "
				<< size;
			el.Push(EL_Warning, oss.str());
		}

		/* Create the kanjidic object with our string data. */
		this->KanjidicParser(rawData, jisStd);

		returnCode = KD_SUCCESS;
		el.Push(EL_Silent, string("Kanji dictionary file \"")
				.append(filename).append("\" loaded successfully."));
	}
	else
		returnCode = KD_FAILURE;

	if(rawData) delete[] rawData;
	return returnCode;
}

int KDict::LoadKanjidic2(const char* filename) {
	int returnCode = KD_FAILURE;
	xmlTextReaderPtr reader;
	xmlChar* ptr;
	int ret;

	/* Vars for navigating through the data */
	string element, d1element;
	map<string, string> attributes;
	map<string, string>::iterator mssi;
	int nodeType;
	bool isAttribute=false;
	/* Var for storing values of the entries */
	string sValue;
	/* GP vars */
	string temp;

	reader = xmlNewTextReaderFilename(filename);
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
				attributes.clear();
				break;
			case XML_READER_TYPE_END_ELEMENT:
				element = (char*)xmlTextReaderName(reader);
				if(element=="character") {
					wchar_t wc = utfconv_mw(k->literal)[0];
					/* End of character entry: append to data list */
					if(!kdictData.assign(wc, *k)) {
						ostringstream oss;
						oss << ERR_PREF
							<< "Error assigning kanjidic2 entry to hash table!";
						el.Push(EL_Error, oss.str());
					}
					delete k;
					k = NULL;
				}
				attributes.clear();
				break;
			case XML_READER_TYPE_ATTRIBUTE:
				temp = (char*)xmlTextReaderName(reader);
				ptr = xmlTextReaderValue(reader);
				attributes[temp] = (char*)ptr;
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
						k->codepoint[attributes["cp_type"]] = sValue;
					} else if(element=="rad_value") {
						temp = attributes["rad_type"];
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
						k->variant[attributes["var_type"]] = sValue;
					} else if(element=="freq") {
						k->freq = atoi(sValue.c_str());
					} else if(element=="rad_name") {
						k->radicalName = sValue;
					} else if(element=="dic_ref") {
						k->dictCode[attributes["dr_type"]] = sValue;
						if(attributes["dr_type"]=="moro"
						   && attributes["m_vol"].length()>0) {
							temp = "V";
							temp.append(attributes["m_vol"]);
							temp.append(1, 'P');
							temp.append(attributes["m_page"]);
							k->dictCode["moro"].append(temp);
						}
					} else if(element=="q_code") {
						if(attributes["qc_type"]=="skip"
						   && attributes["skip_misclass"].length()>0) {
							k->skipMisclass.push_back(
								pair<string,string>(
									attributes["skip_misclass"],
									sValue));
						} else {
							k->queryCode[attributes["qc_type"]] = sValue;
						}
					} else if(element=="reading") {
						temp = attributes["r_type"];
						if(temp=="pinyin") {
							k->pinyin.push_back(sValue);
						} else if(temp=="korean_r") {
							k->korean_r.push_back(sValue);					
						} else if(temp=="korean_h") {
							k->korean_h.push_back(sValue);
						} else if(temp=="ja_on") {
							/* Need to handle r_status and on_type! */
							/* Need to convert xx.x to xx(x) notation. */
							k->onyomi.push_back(sValue);
						} else if(temp=="ja_kun") {
							/* Need to handle r_status! */
							/* Need to convert xx.x to xx(x) notation. */
							k->kunyomi.push_back(sValue);
						} else {
							ostringstream oss;
							oss << ERR_PREF << "Invalid r_type: " << temp;
							el.Push(EL_Error, oss.str());
						}
						/* This section is "to-do" */
					} else if(element=="meaning") {
						temp = attributes["m_lang"];
						if(temp.length()==0) temp = "en";
						k->meaning[temp].push_back(sValue);;
					} else if(element=="nanori") {
						k->nanori.push_back(sValue);
					} else {
						ostringstream oss;
						oss << ERR_PREF << "UNHANDLED element: " << element;
						el.Push(EL_Error, oss.str());
					}
				}
				/* default parsing */
				else {
					/*cout << "DEBUG: Depth 1 element is " << d1element
					  << ", element is " << element
					  << ", value is " << sValue << endl;*/
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
				.append(filename).append("\" loaded successfully."));
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

int KDict::LoadKradfile(const char* filename) {
	int returnCode = KD_FAILURE;
	stringbuf sb;
	ifstream f(filename, ios::in|ios::binary);
	if(f.is_open()) {
		f >> &sb;
		f.close();

		list<wstring> data =
			StrTokenize<wchar_t>(utfconv_mw(sb.str()), L"\n");
		while(data.size()>0) {
			wstring token = data.front();
			data.pop_front();
			if(token.length()>0 && token[0]!=L'#') {
				/* KRADFILE-specific stuff here */
				/* Get rid of the spaces in the string */
				token = TextReplace<wchar_t>(token, L" ", L"");
				/* Now we can easily pull in the data */
				if(!kradData.assign(token[0], token.substr(2))) {
					ostringstream oss;
					oss << ERR_PREF << "KRADFILE: Error assigning ("
						<< utfconv_wm(token.substr(0,1)) << ", "
						<< utfconv_wm(token.substr(2)) << ") to hash table!";
					el.Push(EL_Error, oss.str());
				}
			}
		}

		returnCode = KD_SUCCESS;
		el.Push(EL_Silent, string("Kanji dictionary file \"")
				.append(filename).append("\" loaded successfully."));
	}
	return returnCode;
}

int KDict::LoadRadkfile(const char* filename) {
	int returnCode = KD_FAILURE;
	stringbuf sb;
	ifstream f(filename, ios::in|ios::binary);
	if(f.is_open()) {
		f >> &sb;
		f.close();

		/* RADKFILE entries all start with $.
		   Split on $, and discard the first entry since it is the explanation
		   preceding the first entry. */
		list<wstring> data =
			StrTokenize<wchar_t>(utfconv_mw(sb.str()), L"$");
		data.pop_front();

		while(data.size()>0) {
			wstring entry = data.front();
			data.pop_front();
			if(entry.length()>0 && entry[0]!=L'#') {
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

					if(!radkData.assign(key, value)) {
						ostringstream oss;
						oss << ERR_PREF << "RADKFILE: Error assigning ("
							<< utfconv_wm(wstring().append(1,key)) << ", "
							<< utfconv_wm(value) << ") to hash table!";
						el.Push(EL_Error, oss.str());
					}
					if(!radkDataStrokes.assign(key, strokeCount)) {
						ostringstream oss;
						oss << ERR_PREF << "RADKFILE: Error assigning ("
							<< utfconv_wm(wstring().append(1,key))
							<< ", " << strokeCount << ") to hash table!";
						el.Push(EL_Error, oss.str());
					}
				}
			}
		}

		returnCode = KD_SUCCESS;
		el.Push(EL_Silent, string("Kanji dictionary file \"")
				.append(filename).append("\" loaded successfully."));
	}
	return returnCode;
}

string JisHexToKuten(const string& jisHex) {
	int i;
	stringstream ss(jisHex);
	ss >> hex >> i;
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
	k.codepoint[jisStd] = JisHexToKuten(tl.front()); tl.pop_front();

	/* Now, just loop through the remaining entries in the list. */
	string* ps;
	while(tl.size()>0) {
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
			k.codepoint["ucs"] = ps->substr(1);
			break;
		/* Dictionary codes for most of the following */
		case 'H':
			/* New Japanese-English Character Dictionary (Halpern) */
			k.dictCode["halpern_njecd"] = ps->substr(1);
			break;
		case 'N':
			/* Modern Reader's Japanese-English Character Dictionary (Nelson) */
			k.dictCode["nelson_c"] = ps->substr(1);
			break;
		case 'V':
			/* The New Nelson's Japanese-English Character Dictionary */
			k.dictCode["nelson_n"] = ps->substr(1);
			break;
		case 'P':
			/* SKIP codes. */
			/* Thanks to changes in permissible SKIP code usage (change to
			   Creative Commons licensing in January 2008), we can now use
			   this without problems. */
			k.queryCode["skip"] = ps->substr(1);
			break;
		case 'I':  /* Spahn/Hadamitzky dictionaries */
			if((*ps)[1]=='N') {
				/* Kanji & Kana (Spahn, Hadamitzky) */
				k.dictCode["sh_kk"] = ps->substr(2);
			} else {
				/* Query Code: Kanji Dictionary (Spahn, Hadamitzky) */
				k.queryCode["sh_desc"] = ps->substr(1);
			}
			break;
		case 'Q':
			/* Four Corner code */
			k.queryCode["four_corner"] = ps->substr(1);
			break;
		case 'M':
			if((*ps)[1]=='N') {
				/* Morohashi Daikanwajiten Index */
				k.dictCode["moro"].insert(0, ps->substr(2));
			} else if((*ps)[1]=='P') {
				/* Morohashi Daikanwajiten Volume/Page */
				k.dictCode["moro"]
					.append(1, '/')
					.append(ps->substr(2));
			}
			break;
		case 'E':
			/* A Guide to Remembering Japanese Characters (Henshall) */
			k.dictCode["henshall"] = ps->substr(1);
			break;
		case 'K':
			/* Gakken Kanji Dictionary ("A New Dictionary of Kanji Usage") */
			k.dictCode["gakken"] = ps->substr(1);
			break;
		case 'L':
			/* Remembering the Kanji (Heisig) */
			k.dictCode["heisig"] = ps->substr(1);
			break;
		case 'O':
			/* Japanese Names (O'Neill) */
			k.dictCode["oneill_names"] = ps->substr(1);
			break;
		case 'D':
			switch((*ps)[1]) {
			case 'B':
				/* Japanese for Busy People (AJLT) */
				k.dictCode["busy_people"] = ps->substr(1);
				break;
			case 'C':
				/* The Kanji Way to Japanese Language Power (Crowley) */
				k.dictCode["crowley"] = ps->substr(1);
				break;
			case 'F':
				/* Japanese Kanji Flashcards (White Rabbit Press) */
				k.dictCode["jf_cards"] = ps->substr(1);
				break;
			case 'G':
				/* Kodansha Compact Kanji Guide */
				k.dictCode["kodansha_compact"] = ps->substr(1);
				break;
			case 'H':
				/* A Guide To Reading and Writing Japanese (Henshall) */
				k.dictCode["henshall3"] = ps->substr(1);
				break;
			case 'J':
				/* Kanji in Context (Nishiguchi and Kono) */
				k.dictCode["kanji_in_context"] = ps->substr(1);
				break;
			case 'K':
				/* Kodansha Kanji Learner's Dictionary (Halpern) */
				k.dictCode["halpern_kkld"] = ps->substr(1);
				break;
			case 'O':
				/* Essential Kanji (O'Neill) */
				k.dictCode["oneill_kk"] = ps->substr(1);
				break;
			case 'R':
				/* Query Code: 2001 Kanji (De Roo) */
				k.queryCode["deroo"] = ps->substr(1);
				break;
			case 'S':
				/* A Guide to Reading and Writing Japanese (Sakade) */
				k.dictCode["sakade"] = ps->substr(1);
				break;
			case 'T':
				/* Tuttle Kanji Cards (Kask) */
				k.dictCode["tutt_cards"] = ps->substr(1);
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
				k.variant["deroo"]=ps->substr(2);
				break;	
			case 'H':
				/* NJECD code */
				k.variant["njecd"]=ps->substr(2);
				break;
			case 'I':
				/* S_H code */
				k.variant["s_h"]=ps->substr(2);
				break;
			case 'J':
				/* XJ# = JIS hex code: 0=jis208, 1=jis212, 2=jis213 */
				switch((*ps)[2]) {
				case '0':
					k.variant["jis208"]=JisHexToKuten(ps->substr(3));
					break;
				case '1':
					k.variant["jis212"]=JisHexToKuten(ps->substr(3));
					break;
				case '2':
					k.variant["jis213"]=JisHexToKuten(ps->substr(3));
					break;
				}
				break;
			case 'N':
				/* nelson_c code */
				k.variant["nelson_c"]=ps->substr(2);
				break;
			case 'O':
				/* oneill code */
				k.variant["oneill"]=ps->substr(2);
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
					pair<string,string>("stroke_and_posn", ps->substr(3)));
			else if(sTemp == "ZPP") {
				k.skipMisclass.push_back(
					pair<string,string>("posn", ps->substr(3)));
			} else if(sTemp == "ZRP") {
				k.skipMisclass.push_back(
					pair<string,string>("stroke_diff", ps->substr(3)));
			} else if(sTemp == "ZSP") {
				k.skipMisclass.push_back(
					pair<string,string>("stroke_count", ps->substr(3)));
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
				if(tl.size()==0) break;
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
			k.meaning["en"].push_back(sTemp);
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
			/* Create new KInfo object.
			   If one already exists for this character, copy over the
			   information. */
			KInfo k;
			BoostHM<wchar_t, KInfo>::iterator it = kdictData.find(wToken[0]);
			if(it!=kdictData.end()) k = it->second;
			/* Fill the KInfo structure */
			KanjidicToKInfo(utfconv_wm(wToken), k, jisStd);

			/* Add to hash table */
			if(!kdictData.assign(wToken[0], k)) {
				ostringstream oss;
				string temp = utfconv_wm(wToken);
				oss << ERR_PREF << "Error assigning (" << temp[0]<< ", "
					<< temp << ") to hash table!";
				el.Push(EL_Error, oss.str());
			}
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

wstring KDict::KInfoToHtml(const KInfo& kInfo) {
	Preferences* prefs = Preferences::Get();
	return KInfoToHtml(kInfo,
					   prefs->kanjidicOptions,
					   prefs->kanjidicDictionaries);
}

wstring KDict::KInfoToHtml(const KInfo& kInfo,
						   long options, long dictionaries) {
/*	return wstring(L"<p>")
		.append(s[0])
		.append(L"</p>");*/
	#warning KInfoToHtml currently is unimplemented!
#if 0
	wostringstream result;
	wostringstream header;
	wstring onyomi, kunyomi, nanori, radicalReading, english;
	wstring dictionaryInfo;
	wstring lowRelevance;
	wstring unhandled;
	long grade = -1, frequency = -1, tmode = 0;
	wstring strokes;
	wstring koreanRomanization, pinyinRomanization, crossReferences, miscodes;
	wstring sTemp, token;
	list<wstring> t = StrTokenize<wchar_t>(kanjidicStr, L" ");
	wchar_t c, c2;

	/* Special processing for the first 2 entries of the line. */
	if(t.size()>1) {
		/* header = "<h1><font size=\"-6\">" + args[0] + "</font></h1>"; */
		/*header.append(L"<p style=\"font-size:32pt\">") */
		header << L"<p><font size=\"7\">" << t.front() << L"</font></p>";
		t.pop_front();
		lowRelevance.append(L"<li>JIS code: 0x")
			.append(t.front())
			.append(L"</li>");
		t.pop_front();
	}

	/* NEW!  Temporary code for loading in SODs and SODAs from KanjiCafe! */
	if(options & (KDO_SOD_STATIC | KDO_SOD_ANIM) != 0) {
		string utfStr;
		/* Get a UTF8-encoded string for the kanji. */
		utfStr = utfconv_wm(kanjidicStr.substr(0,1));

		/* Convert to a low-to-high-byte hex string. */
		ostringstream ss;
		for(unsigned int i=0;i<utfStr.length();i++) {
			ss << hex << setw(2) << setfill('0')
			   << (unsigned int)((unsigned char)utfStr[i]);
		}

		wstringstream sod;
		/* Load static SOD, if present */
		if((options & KDO_SOD_STATIC) != 0) {
			Preferences* p = Preferences::Get();
			ostringstream fn;
			string sodDir = p->GetSetting("sod_dir");
			if(sodDir.length()==0) sodDir = "sods";
			fn << sodDir << DSCHAR
			   << "sod-utf8-hex" << DSCHAR
			   << ss.str() << ".png";

#ifdef DEBUG
			printf("DEBUG: Checking for existance of file \"%s\"...\n",
				   fn.str().c_str());
#endif
			ifstream f(fn.str().c_str());
			if(f.is_open()) {
				f.close();
				if(sod.str().length()>0) sod << L"<br />";
				sod << L"<img src=\"" << utfconv_mw(fn.str()) << L"\" />";
			}
		}
		/* Load animated SOD, if present */
		if((options & KDO_SOD_ANIM) != 0) {
			ostringstream fn;
			fn << "sods" << DSCHAR
			   << "soda-utf8-hex" << DSCHAR
			   << ss.str() << ".gif";
#ifdef DEBUG
			printf("DEBUG: Checking for existance of file \"%s\"...\n",
				   fn.str().c_str());
#endif
			ifstream f(fn.str().c_str());
			if(f.is_open()) {
				f.close();
				if(sod.str().length()>0) sod << L"<br />";
				sod << L"<img src=\"" << utfconv_mw(fn.str()) << L"\" />";
			}
		}
		/* Append the chart(s) in a paragraph object. */
		if(sod.str().length()>0) {
			header << L"<p>" << sod.str() <<
				L"<br /><font size=\"1\">(Kanji stroke order graphics "
				L"used under license from KanjiCafe.com.)</font></p>";
		}
	}

	while(t.size()>0) {
		token = t.front();
		t.pop_front();
		sTemp = token;
		c = sTemp[0];
		/* If a preceding character is detected, strip it */
		if(c == L'(' || c == L'〜') {
			sTemp = sTemp.substr(1);
			c = sTemp[0];
		}
		if(tmode==0) {
			if(IsKatakana(c)) {
				/* Onyomi reading detected */
				/*if(onyomi.length()>0) onyomi.append(L"　"); */
				if(onyomi.length()>0) onyomi.append(L"&nbsp; ");
				onyomi.append(token);   /* Copy the original string, including ()'s and 〜's */
				continue;
			}
			else if(IsHiragana(c)) {
				/* Kunyomi reading detected */
				if(kunyomi.length()>0) kunyomi.append(L"&nbsp; ");
				kunyomi.append(token);  /* Copy the original string, including ()'s and 〜's */
				continue;
			}
		} else if(tmode==1) {
			if(IsFurigana(c)) {
				/* Nanori reading detected */
				if(nanori.length()>0) nanori.append(L"&nbsp; ");
				nanori.append(token);   /* Copy the original string, including ()'s and 〜's */
				continue;
			}
		} else if(tmode==2) {
			if(IsFurigana(c)) {
				/* Special radical reading detected */
				if(radicalReading.length()>0) radicalReading.append(L"&nbsp; ");
				radicalReading.append(token);
				continue;
			}
		}
		if(c == L'{') {
			/* English meaning detected
			   Special handling is needed to take care of spaces, though.
			   We'll "cheat" and mess with our iterator a bit if a space is detected. */
			while(t.size()>0 && sTemp[sTemp.length()-1] != L'}') {
				sTemp.append(L" ").append(t.front());
				t.pop_front();
			}
			if(english.length()>0) english.append(L", ");
			english.append(sTemp.substr(1,sTemp.length()-2));  /* Strip the {} */
		}
		else {
			switch(c) {
			case L'T':  /* Change "t mode" */
				/*wstring(sTemp.substr(1)).ToLong(&tmode);*/
				wistringstream(sTemp.substr(1)) >> tmode;
#ifdef DEBUG
				if(tmode>2) printf("WARNING: T-mode set to %d.\nT-modes above 2 are not currently documented!", (int)tmode);
#endif
				break;
			case L'B':  /* Bushu radical */
				lowRelevance.append(L"<li>Bushu radical: ").append(sTemp.substr(1)).append(L"</li>");
				break;
			case L'C':  /* Classical radical */
				lowRelevance.append(L"<li>Classical radical: ").append(sTemp.substr(1)).append(L"</li>");
				break;
			case L'F':  /* Frequency */
				/*wstring(sTemp.substr(1)).ToLong(&frequency);*/
				wistringstream(sTemp.substr(1)) >> frequency;
				break;
			case L'G':  /* Grade level */
				/*wstring(sTemp.substr(1)).ToLong(&grade);*/
				wistringstream(sTemp.substr(1)) >> grade;
				break;
			case L'S':  /* Stroke count */
				if(strokes.length()==0) {
					strokes = sTemp.substr(1);
				} else if(!strokes.find(L' ')!=wstring::npos) {
					strokes.append(L" (Miscounts: ")
						.append(sTemp.substr(1))
						.append(L")");
				} else {
					strokes = strokes.substr(0, strokes.length()-1)
						.append(L", ")
						.append(sTemp.substr(1))
						.append(L")");
				}
				break;
			case L'U':  /* Unicode value */
				lowRelevance.append(L"<li>Unicode: 0x").append(sTemp.substr(1)).append(L"</li>");
				break;
			/* From here, it's all dictionary codes */
			case L'H':
				if((dictionaries & KDD_NJECD)!=0)
					dictionaryInfo.append(L"<li>New Japanese-English Character Dictionary (Halpern): ")
						.append(sTemp.substr(1)).append(L"</li>");
				break;
			case L'N':
				if((dictionaries & KDD_MRJECD)!=0)
					dictionaryInfo.append(L"<li>Modern Reader's Japanese-English Character Dictionary (Nelson): ")
						.append(sTemp.substr(1)).append(L"</li>");
				break;
			case L'V':
				if((dictionaries & KDD_NNJECD)!=0)
					dictionaryInfo.append(L"<li>The New Nelson's Japanese-English Character Dictionary: ")
						.append(sTemp.substr(1)).append(L"</li>");
				break;
			case L'P':
				/* SKIP codes. */
				/* Thanks to changes in permissible SKIP code usage (change to
				   Creative Commons licensing in January 2008), we can now use
				   this without problems. */
				if((dictionaries & KDD_SKIP)!=0)
					dictionaryInfo.append(L"<li>SKIP code: ")
						.append(sTemp.substr(1)).append(L"</li>");
				break;
			case L'I':  /* Spahn/Hadamitzky dictionaries */
				if(sTemp[1]==L'N') {
					if((dictionaries & KDD_KK)!=0) {
						dictionaryInfo.append(L"<li>Kanji & Kana (Spahn, Hadamitzky): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
				} else {
					if((dictionaries & KDD_KD)!=0) {
						dictionaryInfo.append(L"<li>Kanji Dictionary (Spahn, Hadamitzky): ")
							.append(sTemp.substr(1)).append(L"</li>");
					}
				}
				break;
			case L'Q':
				if((dictionaries & KDD_FC)!=0) {
					dictionaryInfo.append(L"<li>Four Corner code: ")
						.append(sTemp.substr(1)).append(L"</li>");
				}
				break;
			case L'M':
				c2 = sTemp[1];
				if(c2==L'N') {
					if((dictionaries & KDD_MOROI)!=0) {
						dictionaryInfo.append(L"<li>Morohashi Daikanwajiten Index: ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
				} else if(c2==L'P') {
					if((dictionaries & KDD_MOROVP)!=0) {
						dictionaryInfo.append(L"<li>Morohashi Daikanwajiten Volume/Page: ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
				}
				break;
			case L'E':
				if((dictionaries & KDD_GRJC)!=0) {
					dictionaryInfo.append(L"<li>A Guide to Remembering Japanese Characters (Henshal): ")
						.append(sTemp.substr(1)).append(L"</li>");
				}
				break;
			case L'K':
				if((dictionaries & KDD_GKD)!=0) {
					dictionaryInfo.append(L"<li>Gakken Kanji Dictionary (\"A New Dictionary of Kanji Usage\"): ")
						.append(sTemp.substr(1)).append(L"</li>");
				}
				break;
			case L'L':
				if((dictionaries & KDD_RTK)!=0) {
					dictionaryInfo.append(L"<li>Remembering the Kanji (Heisig): ")
						.append(sTemp.substr(1)).append(L"</li>");
				}
				break;
			case L'O':
				if((dictionaries & KDD_JN)!=0) {
					dictionaryInfo.append(L"<li>Japanese Names (O'Neill): ")
						.append(sTemp.substr(1)).append(L"</li>");
				}
				break;
			case L'D':
				c2 = sTemp[1];
				switch(c2) {
				case L'B':
					if((dictionaries & KDD_JBP)!=0) {
						dictionaryInfo.append(L"<li>Japanese for Busy People (AJLT): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'C':
					if((dictionaries & KDD_KWJLP)!=0) {
						dictionaryInfo.append(L"<li>The Kanji Way to Japanese Language Power (Crowley): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'F':
					if((dictionaries & KDD_JKF)!=0) {
						dictionaryInfo.append(L"<li>Japanese Kanji Flashcards (White Rabbit Press): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'G':
					if((dictionaries & KDD_KCKG)!=0) {
						dictionaryInfo.append(L"<li>Kodansha Compact Kanji Guide: ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'H':
					if((dictionaries & KDD_GTRWJH)!=0) {
						dictionaryInfo.append(L"<li>A Guide To Reading and Writing Japanese (Hensall): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'J':
					if((dictionaries & KDD_KIC)!=0) {
						dictionaryInfo.append(L"<li>Kanji in Context (Nishiguchi and Kono): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'K':
					if((dictionaries & KDD_KLD)!=0) {
						dictionaryInfo.append(L"<li>Kanji Learner's Dictionary (Halpern): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'O':
					if((dictionaries & KDD_EK)!=0) {
						dictionaryInfo.append(L"<li>Essential Kanji (O'Neill): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'R':
					if((dictionaries & KDD_DR)!=0) {
						dictionaryInfo.append(L"<li>2001 Kanji (De Roo): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'S':
					if((dictionaries & KDD_GTRWJS)!=0) {
						dictionaryInfo.append(L"<li>A Guide to Reading and Writing Japanese (Sakade): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				case L'T':
					if((dictionaries & KDD_TKC)!=0) {
						dictionaryInfo.append(L"<li>Tuttle Kanji Cards (Kask): ")
							.append(sTemp.substr(2)).append(L"</li>");
					}
					break;
				default:
					if(unhandled.length()>0) unhandled.append(L" ");
					unhandled.append(sTemp);
					break;
				}
				break;
			/* Crossreferences and miscodes */
			case L'X':
				if(crossReferences.length()>0) crossReferences.append(L", ");
				crossReferences.append(sTemp.substr(1));
				break;
			case L'Z':
				if(miscodes.length()>0) miscodes.append(L", ");
				miscodes.append(sTemp.substr(1));
				break;
			/* Korean/Pinyin (Chinese) romanization */
			case L'W':
				if(koreanRomanization.length()>0) koreanRomanization.append(L", ");
				koreanRomanization.append(sTemp.substr(1));
				break;
			case L'Y':
				if(pinyinRomanization.length()>0) pinyinRomanization.append(L", ");
				pinyinRomanization.append(sTemp.substr(1));
				break;
			default:
				if(unhandled.length()>0) unhandled.append(L" ");
				unhandled.append(sTemp);
				break;
			}
		}
	} /* while(t.HasMoreTokens()) */

	if(header.str().length() > 0) result << header.str();
#ifdef DEBUG
	printf("DEBUG: header=[%ls]\n", header.str().c_str());
#endif
	result << L"<ul>";
	if((options & KDO_READINGS) != 0) {
		if(onyomi.length() > 0)
			result << L"<li>Onyomi Readings: " << onyomi << L"</li>";
		if(kunyomi.length() > 0)
			result << L"<li>Kunyomi Readings: " << kunyomi << L"</li>";
		if(nanori.length() > 0)
			result << L"<li>Nanori Readings: " << nanori << L"</li>";
		if(radicalReading.length() > 0)
			result << L"<li>Special Radical Reading: " << radicalReading <<
				L"</li>";
	}
	if((options & KDO_MEANINGS) != 0) {
		if(english.length() > 0)
			result << L"<li>English Meanings: " << english << L"</li>";
	}
	if((options & KDO_HIGHIMPORTANCE) != 0) {
		if(strokes.length() > 0)
			result << L"<li>Stroke count: " << strokes << L"</li>";
		else
			result << L"<li>Stroke count: not specified in KANJIDIC</li>";
		result << L"<li>Grade Level: ";
		if(grade<=6 && grade >= 1) {  /* Jouyou (Grade #) */
			result << L"Jouyou (Grade " << grade << L")";
		} else if(grade==8) {  /* Jouyou (General usage) */
			result << L"Jouyou (General usage)";
		} else if(grade==9) {  /* Jinmeiyou (Characters for names) */
			result << L"Jinmeiyou (Characters for names)";
		} else if(grade==-1) {  /* No flag specified in kanjidic string */
			result << L"Unspecified";
		} else {
			result << L"Unhandled grade level (Grade " << grade << L")";
		}
		result << L"</li>";
		if(frequency!=-1)
			result << L"<li>Frequency Ranking: " << frequency << L"</li>";
		else result << L"<li>Frequency Ranking: Unspecified</li>";
	}
	if((options & KDO_DICTIONARIES) != 0) {
		if(dictionaryInfo.length()>0)
			result << L"<li>Dictionary Codes:<ul>" << dictionaryInfo
				   << L"</ul></li>";
	}
	if((options & KDO_VOCABCROSSREF) != 0) {
		vector<wstring> *vList = &(jben->vocabList->GetVocabList());
		wchar_t thisKanji = kanjidicStr[0];
		vector<wstring> crossRefList;
		vector<wstring>::iterator vIt;
		for(vIt=vList->begin(); vIt!=vList->end(); vIt++) {
			if(vIt->find(thisKanji)!=wstring::npos) {
				crossRefList.push_back(*vIt);
			}
		}
		if(crossRefList.size()>0) {
			result << L"<li>This kanji is used by words in your study list:<br><font size=\"7\">";
			vIt = crossRefList.begin();
			result << *vIt;
			for(++vIt; vIt!=crossRefList.end(); vIt++) {
				result << L"&nbsp; " << *vIt;
			}
			result << L"</font></li>";
		}
	}
	if((options & KDO_LOWIMPORTANCE) != 0) {
		if(koreanRomanization.length()>0) lowRelevance.append(L"<li>Korean romanization: ").append(koreanRomanization).append(L"</li>");
		if(pinyinRomanization.length()>0) lowRelevance.append(L"<li>Pinyin romanization: ").append(pinyinRomanization).append(L"</li>");
		if(crossReferences.length()>0) lowRelevance.append(L"<li>Cross reference codes: ").append(crossReferences).append(L"</li>");
		if(miscodes.length()>0) lowRelevance.append(L"<li>Miscodes: ").append(miscodes).append(L"</li>");
		if(lowRelevance.length()>0)
			result << L"<li>Extra Information:<ul>" << lowRelevance
				   << L"</ul></li>";
	}
	if((options & KDO_UNHANDLED) != 0) {
		if(unhandled.length()>0)
			result << L"<li>Unhandled: " << unhandled << L"</li>";
	}
	result << L"</ul>";

	return result.str();
#endif
	return wstring();
}

const BoostHM<wchar_t,KInfo>* KDict::GetHashTable() const {
	return &kdictData;
}

bool KDict::MainDataLoaded() const {
	if(kdictData.size()>0) return true;
	return false;
}

const KInfo* KDict::GetEntry(const wchar_t key) const {
	BoostHM<wchar_t, KInfo>::const_iterator kci = kdictData.find(key);
	if(kci != kdictData.end())
		return &(kci->second);
	return NULL;;
}
