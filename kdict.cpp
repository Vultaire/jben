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

#include "jben.h"
#include "kdict.h"
#include "file_utils.h"
#include "jutils.h"
#include "encoding_convert.h"
#include "string_utils.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <list>
using namespace std;

KDict* KDict::kdictSingleton = NULL;

const KDict *KDict::GetKDict() {
	if(!kdictSingleton)
		kdictSingleton = new KDict;
	return kdictSingleton;
}

KDict::KDict() {
	LoadKanjidic();
	LoadKradfile();
	LoadRadkfile();
}

void KDict::Destroy() {
	if(kdictSingleton) {
		delete kdictSingleton;
		kdictSingleton = NULL;
	}
}

int KDict::LoadKanjidic(const char *filename) {
	char *rawData = NULL;
	unsigned int size;
	int returnCode=0xDEADBEEF;

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
		this->KanjidicParser(rawData);

		returnCode = KD_SUCCESS;
	}
	else
		returnCode = KD_FAILURE;

	if(rawData) delete[] rawData;
	return returnCode;
}

int KDict::LoadKradfile(const char *filename) {
	int returnCode = 0xDEADBEEF;
	return returnCode;
}

int KDict::LoadRadkfile(const char *filename) {
	int returnCode = 0xDEADBEEF;
	return returnCode;
}

/* This could be sped up: copy the first UTF-8 character into a string, then
   run a conversion on that.  Trivial though. */
void KDict::KanjidicParser(char *kanjidicRawData) {
	char *token = strtok(kanjidicRawData, "\n");
	wstring wToken;
	while(token) {
		if( (strlen(token)>0) && (token[0]!='#') ) {
			wToken = ConvertString<char, wchar_t>
				(token, "UTF-8", wcType.c_str());
			/* Convert token to proper format */
			wToken = ConvertKanjidicEntry(wToken);
			/* Add to hash table */
			if(!kanjidicData.assign(wToken[0], token)) {
#ifdef DEBUG
				fprintf(stderr,
					"Error assigning (%lc, %ls) to hash table!\n",
					wToken[0], wToken.c_str());
#endif
			}
		}
		token = strtok(NULL, "\n");
	}
}

KDict::~KDict() {
	/* Currently: nothing here. */
}

/* This function returns a wstring containing the desired line of the
   kanjidic hash table.  A conversion from string to wstring is included
   in this call since strings are only used for more compressed internal
   storage.  This is followed by a slight reformatting of the string for
   better presentation. */
wstring KDict::GetKanjidicStr(wchar_t c) const {
	BoostHM<wchar_t,string>::iterator it = kanjidicData.find(c);
	if(it==kanjidicData.end()) return L"";
	wstring s;
	s = ConvertString<char, wchar_t>(it->second, "UTF-8", wcType.c_str());
	return ConvertKanjidicEntry(s);
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

	/* Second conversion: - to 〜, when a neighboring character is hiragana/katakana */
	index = temp.find(L'-', 0);
	while(index!=wstring::npos) {
		/* Proceed if the character before or after the "-" is hiragana/katakana. */
		if(IsFurigana(temp[index-1]) || IsFurigana(temp[index+1]))
			temp[index]=L'〜';

		lastIndex = index;
		index = temp.find(L'-', lastIndex+1);
	}

	/* Return the converted string */
	return temp;
}

wstring KDict::KanjidicToHtml(const wstring& kanjidicStr) {
	Preferences *prefs = Preferences::GetPrefs();
	return KanjidicToHtml(kanjidicStr,
						  prefs->kanjidicOptions,
						  prefs->kanjidicDictionaries);
}

wstring KDict::KanjidicToHtml(const wstring& kanjidicStr,
							  long options, long dictionaries) {
/*	return wstring(L"<p>")
		.append(s[0])
		.append(L"</p>");*/

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
		utfStr = ConvertString<wchar_t, char>
			(kanjidicStr.substr(0,1), wcType.c_str(), "UTF-8");

		/* Convert to a low-to-high-byte hex string. */
		ostringstream ss;
		for(unsigned int i=0;i<utfStr.length();i++) {
			ss << hex << setw(2) << setfill('0')
			   << (unsigned int)((unsigned char)utfStr[i]);
		}

		wstringstream sod;
		/* Load static SOD, if present */
		if((options & KDO_SOD_STATIC) != 0) {
			ostringstream fn;
			fn << "sods" << DIRSEP
			   << "sod-utf8-hex" << DIRSEP
			   << ss.str() << ".png";
			/*wxFileName fn;
			fn.AppendDir(L"sods");
			fn.AppendDir(L"sod-utf8-hex");
			fn.SetName(ss.str());
			fn.SetExt(L"png");*/
#ifdef DEBUG
			printf("DEBUG: Checking for existance of file \"%s\"...\n", fn.str().c_str());
#endif
			ifstream f(fn.str().c_str());
			if(f.is_open()) {
				f.close();
				if(sod.str().length()>0) sod << L"<br />";
				sod << L"<img src=\""
					<< ConvertString<char,wchar_t>(
						fn.str(),"UTF-8",wcType.c_str())
					<< L"\" />";
			}
			/*if(wxFile::Exists(fn.GetFullPath())) {
				sod << L"<img src=\"" << fn.GetFullPath() << L"\" />";
				}*/
		}
		/* Load animated SOD, if present */
		if((options & KDO_SOD_ANIM) != 0) {
			ostringstream fn;
			fn << "sods" << DIRSEP
			   << "soda-utf8-hex" << DIRSEP
			   << ss.str() << ".gif";
			/*wxFileName fn;
			fn.AppendDir(L"sods");
			fn.AppendDir(L"soda-utf8-hex");
			fn.SetName(sTemp);
			fn.SetExt(L"gif");*/
#ifdef DEBUG
			printf("DEBUG: Checking for existance of file \"%s\"...\n", fn.str().c_str());
#endif
			ifstream f(fn.str().c_str());
			if(f.is_open()) {
				f.close();
				if(sod.str().length()>0) sod << L"<br />";
				sod << L"<img src=\""
					<< ConvertString<char,wchar_t>(
						fn.str(),"UTF-8",wcType.c_str())
					<< L"\" />";
			}
			/*if(wxFile::Exists(fn.GetFullPath())) {
				if(sod.str().length()>0) sod << L"<br />";
				sod << L"<img src=\"" << fn.GetFullPath() << L"\" />";
				}*/
		}
		/* Append the chart(s) in a paragraph object. */
		if(sod.str().length()>0) {
			header << L"<p>" << sod.str() <<
				L"<br /><font size=\"1\">(Kanji stroke order graphics used under license from KanjiCafe.com.)</font></p>";
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
}

int KDict::GetIntField(wchar_t kanji, const wstring& marker) const {
	wstring markerStr, kanjiEntry, temp;
	size_t index=0;
	long value=-1;
	int markerLen;

	markerStr.append(L" ").append(marker);
	markerLen=markerStr.length();

	kanjiEntry = GetKanjidicStr(kanji);
	if(kanjiEntry.length()>0) {
		index = kanjiEntry.find(markerStr);
		if(index!=wstring::npos) {
			temp = kanjiEntry.substr(
				index+markerLen,
				kanjiEntry.find(L" ", index+1) - index - (markerLen-1));
			/*temp.ToLong(&value);*/
			wistringstream(temp) >> value;
		}
	}

	return (int)value;
}

const BoostHM<wchar_t,string>* KDict::GetHashTable() const {
	return &kanjidicData;
}

enum {
	KDR_Onyomi=1,
	KDR_Kunyomi,
	KDR_English
};

wstring KDict::GetOnyomiStr(wchar_t c) const {
	return GetKanjidicReading(c, KDR_Onyomi);
}

wstring KDict::GetKunyomiStr(wchar_t c) const {
	return GetKanjidicReading(c, KDR_Kunyomi);
}

wstring KDict::GetEnglishStr(wchar_t c) const {
	return GetKanjidicReading(c, KDR_English);
}

wstring KDict::GetKanjidicReading(wchar_t c, int readingType) const {
	wostringstream result;
	wstring kanjidicStr = GetKanjidicStr(c);

	long tmode = 0;
	wstring sTemp, token;
	list<wstring> t = StrTokenize<wchar_t>(kanjidicStr, L" ");

	/* The first two tokens are guaranteed not to be what we're looking for.  Skip them. */
	if(t.size()>1) {
		t.pop_front();
		t.pop_front();
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
			if(IsKatakana(c) && readingType==KDR_Onyomi) {
				/* Onyomi reading detected */
				if(result.str().length()>0) result << L"  ";
				result << token;   /* Copy the original string,
									  including ()'s and 〜's */
				continue;
			}
			else if(IsHiragana(c) && readingType==KDR_Kunyomi) {
				/* Kunyomi reading detected */
				if(result.str().length()>0) result << L"  ";
				result << token;   /* Copy the original string,
									  including ()'s and 〜's */
				continue;
			}
		}
		if(c == L'{' && readingType==KDR_English) {
			/* English meaning detected
			   Special handling is needed to take care of spaces, though.
			   We'll "cheat" and mess with our iterator a bit if a space is detected. */
			while(t.size()>0 && sTemp[sTemp.length()-1] != L'}') {
				sTemp.append(L" ").append(t.front());
				t.pop_front();
			}
			if(result.str().length()>0) result << L", ";
			result << sTemp.substr(1,sTemp.length()-2);  /* Strip the {} */
		}
		else if(c==L'T') {
			/*wstring(sTemp.substr(1)).ToLong(&tmode);*/
			wistringstream(sTemp.substr(1)) >> tmode;
		}
	}

	return result.str();
}

bool KDict::MainDataLoaded() const {
	if(kanjidicData.size()>0) return true;
	return false;
}
