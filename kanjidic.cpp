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

#include "kanjidic.h"
#include "file_utils.h"
#include "jutils.h"
#include "wx/tokenzr.h"
#include "wx/file.h"
#include "global.h"
#include <fstream>
using namespace std;

Kanjidic *Kanjidic::LoadKanjidic(const char *filename, int& returnCode) {
	Kanjidic *k=NULL;
	char *rawData = NULL;
	unsigned int size;

	ifstream ifile(filename, ios::ate); /* "at end" to get our file size */
	if(ifile) {
		size = ifile.tellg();
		ifile.seekg(0);
		rawData = new char[size+1];
		rawData[size] = '\0';
		ifile.read(rawData, size);
#ifdef DEBUG
		if(strlen(rawData)!=size)
			fprintf(stderr,
			  "WARNING: kanjidic file size: %d, read-in string: %d\n",
			  strlen(rawData),
			  size);
#endif

		/* Create the kanjidic object with our string data. */
		k = new Kanjidic(rawData);

		returnCode = KD_SUCCESS;
	}
	else
		returnCode = KD_FAILURE;

	if(rawData) delete[] rawData;
	return k;
}

/* This could be sped up: copy the first UTF-8 character into a string, then
   run a conversion on that.  Trivial though. */
Kanjidic::Kanjidic(char *kanjidicRawData) {
	char *token = strtok(kanjidicRawData, "\n");
	wxString wxToken;
	while(token) {
		if( (strlen(token)>0) && (token[0]!='#') ) {
			UTF8ToWx(token, wxToken);
			/* Convert token to proper format */
			wxToken = ConvertKanjidicEntry(wxToken);
			/* Add to hash table */
			if(!kanjiHash.assign(wxToken[0], token)) {
#ifdef DEBUG
				fprintf(stderr,
					"Error assigning (%lc, %ls) to hash table!\n",
					wxToken[0], wxToken.c_str());
#endif
			}
		}
		token = strtok(NULL, "\n");
	}
}

Kanjidic::~Kanjidic() {
	/* Currently: nothing here. */
}

/* This function returns a wxString containing the desired line of the
   kanjidic hash table.  A conversion from string to wxString is included
   in this call since strings are only used for more compressed internal
   storage.  This is followed by a slight reformatting of the string for
   better presentation. */
wxString Kanjidic::GetKanjidicStr(wxChar c) {
	BoostHM<wxChar,string>::iterator it = kanjiHash.find(c);
	if(it==kanjiHash.end()) return _T("");
	wxString s;
	UTF8ToWx(it->second, s);
	return ConvertKanjidicEntry(s);
}

/*
 * Performs transformations on a KANJIDIC string for our internal usage.
 * Currently, this includes the following:
 * - Changing あ.いう notation to あ(いう), a la JWPce/JFC.
 * - Changing -あい notation to 〜あい, also a la JWPce/JFC.
 */
wxString Kanjidic::ConvertKanjidicEntry(const wxString& s) {
	size_t index, lastIndex;
	wxString temp = s;

	/* First conversion: あ.いう to あ(いう) */
	index = temp.find(_T('.'), 0);
	while(index!=wxString::npos) {
		/* Proceed if the character preceding the "." is hiragana/katakana. */
		if(IsFurigana(temp[index-1])) {
			temp[index] = _T('(');
			index = temp.find(_T(' '), index+1);
			if(index==wxString::npos) {
				temp.append(_T(')'));
				break;
			} else
				temp.insert(index, _T(')'));
		}
		lastIndex = index;
		index = temp.find(_T('.'), lastIndex+1);
	}

	/* Second conversion: - to 〜, when a neighboring character is hiragana/katakana */
	index = temp.find(_T('-'), 0);
	while(index!=wxString::npos) {
		/* Proceed if the character before or after the "-" is hiragana/katakana. */
		if(IsFurigana(temp[index-1]) || IsFurigana(temp[index+1]))
			temp[index]=_T('〜');

		lastIndex = index;
		index = temp.find(_T('-'), lastIndex+1);
	}

	/* Return the converted string */
	return temp;
}

wxString Kanjidic::KanjidicToHtml(const wxString& kanjidicStr) const {
	return KanjidicToHtml(kanjidicStr, jben->prefs->kanjidicOptions, jben->prefs->kanjidicDictionaries);
}

wxString Kanjidic::KanjidicToHtml(const wxString& kanjidicStr, long options, long dictionaries) const {
/*	return wxString(_T("<p>"))
		.append(s[0])
		.append(_T("</p>"));*/

	wxString result;

	wxString header, onyomi, kunyomi, nanori, radicalReading, english;
	wxString dictionaryInfo;
	wxString lowRelevance;
	wxString unhandled;
	long grade = -1, frequency = -1, tmode = 0;
	wxString strokes;
	wxString koreanRomanization, pinyinRomanization, crossReferences, miscodes;
	wxString sTemp, token;
	wxStringTokenizer t(kanjidicStr, _T(' '));
	wxChar c, c2;

	/* Special processing for the first 2 entries of the line. */
	if(t.CountTokens()>1) {
		/* header = "<h1><font size=\"-6\">" + args[0] + "</font></h1>"; */
		/*header.append(_T("<p style=\"font-size:32pt\">")) */
		header.append(_T("<p><font size=\"7\">"))
			.append(t.GetNextToken())
			.append(_T("</font></p>"));
		lowRelevance.append(_T("<li>JIS code: 0x"))
			.append(t.GetNextToken())
			.append(_T("</li>"));
	}

	/* NEW!  Temporary code for loading in SODs and SODAs from KanjiCafe! */
	if(options & (KDO_SOD_STATIC | KDO_SOD_ANIM) != 0) {
		wxCSConv transcoder(_T("utf-8"));
		if(transcoder.IsOk()) {
			string utfStr;
			/* Get a UTF8-encoded string for the kanji. */
			WxToUTF8(kanjidicStr[0], utfStr);
			sTemp.clear();

			/* Convert to a low-to-high-byte hex string. */
			for(unsigned int i=0;i<utfStr.length();i++) {
				sTemp.Append(
					wxString::Format(_T("%02x"),
									 (unsigned char)utfStr[i]));
			}

			wxString sod;
			/* Load static SOD, if present */
			if((options & KDO_SOD_STATIC) != 0) {
				wxFileName fn;
				fn.AppendDir(_T("sods"));
				fn.AppendDir(_T("sod-utf8-hex"));
				fn.SetName(sTemp);
				fn.SetExt(_T("png"));
#ifdef DEBUG
				printf("DEBUG: Checking for existance of file \"%ls\"...\n", fn.GetFullPath().c_str());
#endif
				if(wxFile::Exists(
		  			fn.GetFullPath()
		  		)) {
					sod.append(wxString::Format(
						_T("<img src=\"%s\" />"), fn.GetFullPath().c_str()
					));
				}
			}
			/* Load animated SOD, if present */
			if((options & KDO_SOD_ANIM) != 0) {
				wxFileName fn;
				fn.AppendDir(_T("sods"));
				fn.AppendDir(_T("soda-utf8-hex"));
				fn.SetName(sTemp);
				fn.SetExt(_T("gif"));
#ifdef DEBUG
				printf("DEBUG: Checking for existance of file \"%ls\"...\n", fn.GetFullPath().c_str());
#endif
				if(wxFile::Exists(
		  			fn.GetFullPath()
		  		)) {
		  			if(sod.length()>0) sod.append(_T("<br />"));
					sod.append(wxString::Format(
						_T("<img src=\"%s\" />"), fn.GetFullPath().c_str()
					));
				}
			}
			/* Append the chart(s) in a paragraph object. */
			if(sod.length()>0) {
				header.append(wxString::Format(
					_T("<p>%s<br /><font size=\"1\">(Kanji stroke order graphics used under license from KanjiCafe.com.)</font></p>"), sod.c_str()
				));
			}
		} else {
			fprintf(stderr, "[%s:%d]: Bad transcoder selected!\n", __FILE__, __LINE__);
		}
	}
	/* END OF EXPERIMENTAL NEW CODE */

	while(t.HasMoreTokens()) {
		token = t.GetNextToken();
		sTemp = token;
		c = sTemp[0];
		/* If a preceding character is detected, strip it */
		if(c == _T('(') || c == _T('〜')) {
			sTemp = sTemp.substr(1);
			c = sTemp[0];
		}
		if(tmode==0) {
			if(IsKatakana(c)) {
				/* Onyomi reading detected */
				/*if(onyomi.length()>0) onyomi.append(_T("　")); */
				if(onyomi.length()>0) onyomi.append(_T("&nbsp; "));
				onyomi.append(token);   /* Copy the original string, including ()'s and 〜's */
				continue;
			}
			else if(IsHiragana(c)) {
				/* Kunyomi reading detected */
				if(kunyomi.length()>0) kunyomi.append(_T("&nbsp; "));
				kunyomi.append(token);  /* Copy the original string, including ()'s and 〜's */
				continue;
			}
		} else if(tmode==1) {
			if(IsFurigana(c)) {
				/* Nanori reading detected */
				if(nanori.length()>0) nanori.append(_T("&nbsp; "));
				nanori.append(token);   /* Copy the original string, including ()'s and 〜's */
				continue;
			}
		} else if(tmode==2) {
			if(IsFurigana(c)) {
				/* Special radical reading detected */
				if(radicalReading.length()>0) radicalReading.append(_T("&nbsp; "));
				radicalReading.append(token);
				continue;
			}
		}
		if(c == _T('{')) {
			/* English meaning detected
			   Special handling is needed to take care of spaces, though.
			   We'll "cheat" and mess with our iterator a bit if a space is detected. */
			while(t.HasMoreTokens() && sTemp[sTemp.length()-1] != _T('}')) {
				sTemp.append(_T(" ")).append(t.GetNextToken());
			}
			if(english.length()>0) english.append(_T(", "));
			english.append(sTemp.substr(1,sTemp.length()-2));  /* Strip the {} */
		}
		else {
			switch(c) {
			case _T('T'):  /* Change "t mode" */
				/* Note: substr() returns type wxStringBase, which disallows access to wxString::ToLong.
				   So, by making a copy of wxString and performing the conversion in the copy, we get around this.
				   This ugly kludge is repeated twice below for frequency and grade level. */
				wxString(sTemp.substr(1)).ToLong(&tmode);
#ifdef DEBUG
				if(tmode>2) printf("WARNING: T-mode set to %d.\nT-modes above 2 are not currently documented!", (int)tmode);
#endif
				break;
			case _T('B'):  /* Bushu radical */
				lowRelevance.append(_T("<li>Bushu radical: ")).append(sTemp.substr(1)).append(_T("</li>"));
				break;
			case _T('C'):  /* Classical radical */
				lowRelevance.append(_T("<li>Classical radical: ")).append(sTemp.substr(1)).append(_T("</li>"));
				break;
			case _T('F'):  /* Frequency */
				wxString(sTemp.substr(1)).ToLong(&frequency);
				break;
			case _T('G'):  /* Grade level */
				wxString(sTemp.substr(1)).ToLong(&grade);
				break;
			case _T('S'):  /* Stroke count */
				if(strokes.length()==0) {
					strokes = sTemp.substr(1);
				} else if(!strokes.find(_T(' '))!=wxString::npos) {
					strokes.append(_T(" (Miscounts: "))
						.append(sTemp.substr(1))
						.append(_T(")"));
				} else {
					strokes = strokes.substr(0, strokes.length()-1)
						.append(_T(", "))
						.append(sTemp.substr(1))
						.append(_T(")"));
				}
				break;
			case _T('U'):  /* Unicode value */
				lowRelevance.append(_T("<li>Unicode: 0x")).append(sTemp.substr(1)).append(_T("</li>"));
				break;
			/* From here, it's all dictionary codes */
			case _T('H'):
				if((dictionaries & KDD_NJECD)!=0)
					dictionaryInfo.append(_T("<li>New Japanese-English Character Dictionary (Halpern): "))
						.append(sTemp.substr(1)).append(_T("</li>"));
				break;
			case _T('N'):
				if((dictionaries & KDD_MRJECD)!=0)
					dictionaryInfo.append(_T("<li>Modern Reader's Japanese-English Character Dictionary (Nelson): "))
						.append(sTemp.substr(1)).append(_T("</li>"));
				break;
			case _T('V'):
				if((dictionaries & KDD_NNJECD)!=0)
					dictionaryInfo.append(_T("<li>The New Nelson's Japanese-English Character Dictionary: "))
						.append(sTemp.substr(1)).append(_T("</li>"));
				break;
			case _T('P'):
				/* SKIP codes. */
				/* This is a thorny issue.  If we want to include a stock KANJIDIC, then we */
				/* need to add encryption to the file and prevent copy/pasting of that data. */
				/* I'll comply later on, but for now I'll use a stripped KANJIDIC. */
#ifdef USE_SKIP
				if((dictionaries & KDD_SKIP)!=0)
					dictionaryInfo.append(_T("<li>SKIP code: "))
						.append(sTemp.substr(1)).append(_T("</li>"));
#endif
				break;
			case _T('I'):  /* Spahn/Hadamitzky dictionaries */
				if(sTemp[1]==_T('N')) {
					if((dictionaries & KDD_KK)!=0) {
						dictionaryInfo.append(_T("<li>Kanji & Kana (Spahn, Hadamitzky): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
				} else {
					if((dictionaries & KDD_KD)!=0) {
						dictionaryInfo.append(_T("<li>Kanji Dictionary (Spahn, Hadamitzky): "))
							.append(sTemp.substr(1)).append(_T("</li>"));
					}
				}
				break;
			case _T('Q'):
				if((dictionaries & KDD_FC)!=0) {
					dictionaryInfo.append(_T("<li>Four Corner code: "))
						.append(sTemp.substr(1)).append(_T("</li>"));
				}
				break;
			case _T('M'):
				c2 = sTemp[1];
				if(c2==_T('N')) {
					if((dictionaries & KDD_MOROI)!=0) {
						dictionaryInfo.append(_T("<li>Morohashi Daikanwajiten Index: "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
				} else if(c2==_T('P')) {
					if((dictionaries & KDD_MOROVP)!=0) {
						dictionaryInfo.append(_T("<li>Morohashi Daikanwajiten Volume/Page: "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
				}
				break;
			case _T('E'):
				if((dictionaries & KDD_GRJC)!=0) {
					dictionaryInfo.append(_T("<li>A Guide to Remembering Japanese Characters (Henshal): "))
						.append(sTemp.substr(1)).append(_T("</li>"));
				}
				break;
			case _T('K'):
				if((dictionaries & KDD_GKD)!=0) {
					dictionaryInfo.append(_T("<li>Gakken Kanji Dictionary (\"A New Dictionary of Kanji Usage\"): "))
						.append(sTemp.substr(1)).append(_T("</li>"));
				}
				break;
			case _T('L'):
				if((dictionaries & KDD_RTK)!=0) {
					dictionaryInfo.append(_T("<li>Remembering the Kanji (Heisig): "))
						.append(sTemp.substr(1)).append(_T("</li>"));
				}
				break;
			case _T('O'):
				if((dictionaries & KDD_JN)!=0) {
					dictionaryInfo.append(_T("<li>Japanese Names (O'Neill): "))
						.append(sTemp.substr(1)).append(_T("</li>"));
				}
				break;
			case _T('D'):
				c2 = sTemp[1];
				switch(c2) {
				case _T('B'):
					if((dictionaries & KDD_JBP)!=0) {
						dictionaryInfo.append(_T("<li>Japanese for Busy People (AJLT): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('C'):
					if((dictionaries & KDD_KWJLP)!=0) {
						dictionaryInfo.append(_T("<li>The Kanji Way to Japanese Language Power (Crowley): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('F'):
					if((dictionaries & KDD_JKF)!=0) {
						dictionaryInfo.append(_T("<li>Japanese Kanji Flashcards (White Rabbit Press): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('G'):
					if((dictionaries & KDD_KCKG)!=0) {
						dictionaryInfo.append(_T("<li>Kodansha Compact Kanji Guide: "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('H'):
					if((dictionaries & KDD_GTRWJH)!=0) {
						dictionaryInfo.append(_T("<li>A Guide To Reading and Writing Japanese (Hensall): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('J'):
					if((dictionaries & KDD_KIC)!=0) {
						dictionaryInfo.append(_T("<li>Kanji in Context (Nishiguchi and Kono): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('K'):
					if((dictionaries & KDD_KLD)!=0) {
						dictionaryInfo.append(_T("<li>Kanji Learner's Dictionary (Halpern): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('O'):
					if((dictionaries & KDD_EK)!=0) {
						dictionaryInfo.append(_T("<li>Essential Kanji (O'Neill): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('R'):
					if((dictionaries & KDD_DR)!=0) {
						dictionaryInfo.append(_T("<li>2001 Kanji (De Roo): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('S'):
					if((dictionaries & KDD_GTRWJS)!=0) {
						dictionaryInfo.append(_T("<li>A Guide to Reading and Writing Japanese (Sakade): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				case _T('T'):
					if((dictionaries & KDD_TKC)!=0) {
						dictionaryInfo.append(_T("<li>Tuttle Kanji Cards (Kask): "))
							.append(sTemp.substr(2)).append(_T("</li>"));
					}
					break;
				default:
					if(unhandled.length()>0) unhandled.append(_T(" "));
					unhandled.append(sTemp);
					break;
				}
				break;
			/* Crossreferences and miscodes */
			case _T('X'):
				if(crossReferences.length()>0) crossReferences.append(_T(", "));
				crossReferences.append(sTemp.substr(1));
				break;
			case _T('Z'):
				if(miscodes.length()>0) miscodes.append(_T(", "));
				miscodes.append(sTemp.substr(1));
				break;
			/* Korean/Pinyin (Chinese) romanization */
			case _T('W'):
				if(koreanRomanization.length()>0) koreanRomanization.append(_T(", "));
				koreanRomanization.append(sTemp.substr(1));
				break;
			case _T('Y'):
				if(pinyinRomanization.length()>0) pinyinRomanization.append(_T(", "));
				pinyinRomanization.append(sTemp.substr(1));
				break;
			default:
				if(unhandled.length()>0) unhandled.append(_T(" "));
				unhandled.append(sTemp);
				break;
			}
		}
	} /* while(t.HasMoreTokens()) */

	if(header.length() > 0) result.append(header);
#ifdef DEBUG
	printf("DEBUG: header=[%ls]\n", header.c_str());
#endif
	result.append(_T("<ul>"));
	if((options & KDO_READINGS) != 0) {
		if(onyomi.length() > 0) result.append(_T("<li>Onyomi Readings: ")).append(onyomi).append(_T("</li>"));
		if(kunyomi.length() > 0) result.append(_T("<li>Kunyomi Readings: ")).append(kunyomi).append(_T("</li>"));
		if(nanori.length() > 0) result.append(_T("<li>Nanori Readings: ")).append(nanori).append(_T("</li>"));
		if(radicalReading.length() > 0) result.append(_T("<li>Special Radical Reading: ")).append(radicalReading).append(_T("</li>"));
	}
	if((options & KDO_MEANINGS) != 0) {
		if(english.length() > 0) result.append(_T("<li>English Meanings: ")).append(english).append(_T("</li>"));
	}
	if((options & KDO_HIGHIMPORTANCE) != 0) {
		if(strokes.length() > 0)
			result.append(_T("<li>Stroke count: ")).append(strokes).append(_T("</li>"));
		else
			result.append(_T("<li>Stroke count: not specified in KANJIDIC"));
		result.append(_T("<li>Grade Level: "));
		if(grade<=6 && grade >= 1) {  /* Jouyou (Grade #) */
			result.append(_T("Jouyou (Grade "))
				.append(wxString::Format(_T("%d"), (int)grade))
				.append(_T(")"));
		} else if(grade==8) {  /* Jouyou (General usage) */
			result.append(_T("Jouyou (General usage)"));
		} else if(grade==9) {  /* Jinmeiyou (Characters for names) */
			result.append(_T("Jinmeiyou (Characters for names)"));
		} else if(grade==-1) {  /* No flag specified in kanjidic string */
			result.append(_T("Unspecified"));
		} else {
			result.append(_T("Unhandled grade level (Grade "))
				.append(wxString::Format(_T("%d"), (int)grade))
				.append(_T(")"));
		}
		result.append(_T("</li>"));
		if(frequency!=-1)
			result.append(_T("<li>Frequency Ranking: "))
				.append(wxString::Format(_T("%d"), (int)frequency))
				.append(_T("</li>"));
		else result.append(_T("<li>Frequency Ranking: Unspecified</li>"));
	}
	if((options & KDO_DICTIONARIES) != 0) {
		if(dictionaryInfo.length()>0) result.append(_T("<li>Dictionary Codes:<ul>")).append(dictionaryInfo).append(_T("</ul></li>"));
	}
	if((options & KDO_VOCABCROSSREF) != 0) {
		vector<wxString> *vList = &(jben->vocabList->GetVocabList());
		wxChar thisKanji = kanjidicStr[0];
		vector<wxString> crossRefList;
		vector<wxString>::iterator vIt;
		for(vIt=vList->begin(); vIt!=vList->end(); vIt++) {
			if(vIt->find(thisKanji)!=wxString::npos) {
				crossRefList.push_back(*vIt);
			}
		}
		if(crossRefList.size()>0) {
			result.append(_T("<li>This kanji is used by words in your study list:<br><font size=\"7\">"));
			vIt = crossRefList.begin();
			result.append(*vIt);
			for(++vIt; vIt!=crossRefList.end(); vIt++) {
				result.append(_T("&nbsp; ")).append(*vIt);
			}
			result.append(_T("</font></li>"));
		}
	}
	if((options & KDO_LOWIMPORTANCE) != 0) {
		if(koreanRomanization.length()>0) lowRelevance.append(_T("<li>Korean romanization: ")).append(koreanRomanization).append(_T("</li>"));
		if(pinyinRomanization.length()>0) lowRelevance.append(_T("<li>Pinyin romanization: ")).append(pinyinRomanization).append(_T("</li>"));
		if(crossReferences.length()>0) lowRelevance.append(_T("<li>Cross reference codes: ")).append(crossReferences).append(_T("</li>"));
		if(miscodes.length()>0) lowRelevance.append(_T("<li>Miscodes: ")).append(miscodes).append(_T("</li>"));
		if(lowRelevance.length()>0) result.append(_T("<li>Extra Information:<ul>")).append(lowRelevance).append(_T("</ul></li>"));
	}
	if((options & KDO_UNHANDLED) != 0) {
		if(unhandled.length()>0) result.append(_T("<li>Unhandled: ")).append(unhandled).append(_T("</li>"));
	}
	result.append(_T("</ul>"));

	return result;
}

int Kanjidic::GetIntField(wxChar kanji, const wxString& marker) const {
	wxString markerStr, kanjiEntry, temp;
	size_t index=0;
	long value=-1;
	int markerLen;

	markerStr.Printf(_T(" %s"), marker.c_str());
	markerLen=markerStr.length();

	kanjiEntry = GetKanjidicStr(kanji);
	if(kanjiEntry.length()>0) {
		index = kanjiEntry.find(markerStr);
		if(index!=wxString::npos) {
			temp = kanjiEntry.substr(
				index+markerLen,
				kanjiEntry.find(_T(" "), index+1) - index - (markerLen-1));
			temp.ToLong(&value);
		}
	}

	return (int)value;
}

const BoostHM<wxChar,string>* const Kanjidic::GetHashTable() const {
	return &kanjiHash;
}

enum {
	KDR_Onyomi=1,
	KDR_Kunyomi,
	KDR_English
};

wxString Kanjidic::GetOnyomiStr(wxChar c) const {
	return GetKanjidicReading(c, KDR_Onyomi);
}

wxString Kanjidic::GetKunyomiStr(wxChar c) const {
	return GetKanjidicReading(c, KDR_Kunyomi);
}

wxString Kanjidic::GetEnglishStr(wxChar c) const {
	return GetKanjidicReading(c, KDR_English);
}

wxString Kanjidic::GetKanjidicReading(wxChar c, int readingType) {
	wxString result;
	wxString kanjidicStr = GetKanjidicStr(c);

	long tmode = 0;
	wxString sTemp, token;
	wxStringTokenizer t(kanjidicStr, _T(' '));

	/* The first two tokens are guaranteed not to be what we're looking for.  Skip them. */
	if(t.CountTokens()>1) {
		t.GetNextToken();
		t.GetNextToken();
	}
	while(t.HasMoreTokens()) {
		token = t.GetNextToken();
		sTemp = token;
		c = sTemp[0];
		/* If a preceding character is detected, strip it */
		if(c == _T('(') || c == _T('〜')) {
			sTemp = sTemp.substr(1);
			c = sTemp[0];
		}
		if(tmode==0) {
			if(IsKatakana(c) && readingType==KDR_Onyomi) {
				/* Onyomi reading detected */
				if(result.length()>0) result.append(_T("  "));
				result.append(token);   /* Copy the original string, including ()'s and 〜's */
				continue;
			}
			else if(IsHiragana(c) && readingType==KDR_Kunyomi) {
				/* Kunyomi reading detected */
				if(result.length()>0) result.append(_T("  "));
				result.append(token);  /* Copy the original string, including ()'s and 〜's */
				continue;
			}
		}
		if(c == _T('{') && readingType==KDR_English) {
			/* English meaning detected
			   Special handling is needed to take care of spaces, though.
			   We'll "cheat" and mess with our iterator a bit if a space is detected. */
			while(t.HasMoreTokens() && sTemp[sTemp.length()-1] != _T('}')) {
				sTemp.append(_T(" ")).append(t.GetNextToken());
			}
			if(result.length()>0) result.append(_T(", "));
			result.append(sTemp.substr(1,sTemp.length()-2));  /* Strip the {} */
		}
		else if(c==_T('T')) wxString(sTemp.substr(1)).ToLong(&tmode);
	}

	return result;
}
