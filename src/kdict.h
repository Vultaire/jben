/*
Project: J-Ben
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File:         kdict.h
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

#ifndef kdict_h
#define kdict_h

#define KD_SUCCESS 0x0
#define KD_FAILURE 0x80000000

/* Onyomi/kunyomi/nanori/radical readings */
#define KDO_READINGS       0x1
/* English meanings */
#define KDO_MEANINGS       0x2
/* Jouyou grade, JLPT level, frequency, stroke count(s) */
#define KDO_HIGHIMPORTANCE 0x4
/* Enable dictionary display */
#define KDO_DICTIONARIES   0x8
/* Less important miscellanea, plus radical #'s */
#define KDO_LOWIMPORTANCE 0x10
/* Enable cross-referencing with vocab study list */
#define KDO_VOCABCROSSREF 0x20
/* KanjiCafe.com Stroke Order Diagrams (if present) */
#define KDO_SOD_STATIC    0x40
#define KDO_SOD_ANIM      0x80
/* Multi-radical information */
#define KDO_MULTIRAD     0x100
/* All options (except UNHANDLED) */
#define KDO_ALL          0x1FF
/* All unhandled dictionary stuff */
#define KDO_UNHANDLED   0x8000

/* Hn...: "New Japanese-English Character Dictionary" by Jack Halpern.  "Halpern" field 1 in JWPce. */
#define KDD_NJECD  0x1
/* Nn...: "Modern Reader's Japanese-English Character Dictionary" by Andrew Nelson.  "Nelson" field 1 in JWPce. */
#define KDD_MRJECD 0x2
/* Vn...: "The New Nelson Japanese-English Character Dictionary" by John Haig.  "Nelson" field 2 in JWPce. */
#define KDD_NNJECD 0x4
/* More "D" codes... add as needed */
/* DBnnn: Japanese for Busy People, AJLT */
#define KDD_JBP    0x8
/* DCnnnn: The Kanji Way to Japanese Language Power, Dale Crowley */
#define KDD_KWJLP  0x10
/* DGnnn: Kodansha Compact Kanji Guide */
#define KDD_KCKG   0x20
/* DHnnnn: A Guide To Reading and Writing Japanese, Ken Henshall et al. */
#define KDD_GTRWJH 0x40
/* DJnnnn: Kanji in Context, Nishiguchi and Kono */
#define KDD_KIC    0x80
/* DKnnnn: Kanji Learners Dictionary, Jack Halpern */
#define KDD_KLD    0x100
/* DOnnnn: Essential Kanji, P.G. O'Neill */
#define KDD_EK     0x200
/* DRnnnn: 2001 Kanji, Father Joseph De Roo */
#define KDD_DR     0x400
/* DSnnnn: A Guide To Reading and Writing Japanese, Florence Sakade */
#define KDD_GTRWJS 0x800
/* DTnnn: Tuttle Kanji Cards, Alexander Kask */
#define KDD_TKC    0x1000
/* DFnnnn: Japanese Kanji Flashcards, White Rabbit Press */
#define KDD_JKF    0x2000
/* "Pn...-n...-n..." - SKIP codes used by Halpern dictionaries */
#define KDD_SKIP   0x4000
/* Inxnn.n: Kanji Dictionary, Spahn & Hadamitzky */
#define KDD_KD     0x8000
/* INnnnn: Kanji & Kana, Spahn & Hadamitzky */
#define KDD_KK     0x10000
/* Qnnnn.n: Four Corner code, used by various dictionaries in China and Japan */
#define KDD_FC     0x20000
/* MNnnnnnnn: Morohashi Daikanwajiten, index number */
#define KDD_MOROI  0x40000
/* MPnn.nnnn: Morohashi Daikanwajiten, volume.page number */
#define KDD_MOROVP 0x80000
/* Morohashi Daikanwajiten (Both indexing options) */
#define KDD_MORO   (KDD_MOROI | KDD_MOROVP)
/* Ennnn: A Guide to Remembering Japanese Characters, Kenneth G. Henshall */
#define KDD_GRJC   0x100000
/* Knnnn: Gakken Kanji Dictionary ("A New Dictionary of Kanji Usage") */
#define KDD_GKD    0x200000
/* Lnnnn: Remembering The Kanji, James Heisig */
#define KDD_RTK    0x400000
/* Onnnn[A]: Japanese Names, P.G. O'Neill */
#define KDD_JN     0x800000
/* Show EVERYTHING!
   This is a lot of output, so it's probably for debug purposes only. */
#define KDD_ALL    0xFFFFFFFF

#include "kdict_classes.h"
#include <string>
#include <list>
#include <tr1/unordered_map>
using namespace std;
using namespace tr1;

class KDict {
public:
	static const KDict *Get();
	static void Destroy();
	~KDict();

	/* General purpose access functions */
	static wstring KInfoToHtml(const KInfo& k);
	static wstring KInfoToHtml(const KInfo& k,
							   long options, long dictionaries);
	static string KInfoToTextBuf(const KInfo& k);
	static string KInfoToTextBuf(const KInfo& k,
								  long options, long dictionaries);
	const KInfo* GetEntry(const wchar_t key) const;
	const unordered_map<wchar_t, KInfo>* GetHashTable() const;

	/* Other functions */
	bool MainDataLoaded() const;
	static string GetSodHtml(const KInfo& k, long options);
	static string GetSodFileName(const KInfo& k, long options);
private:
	/* Hidden constructor */
	KDict();

	/* Dictionary file loaders */
	int LoadKanjidic(const string& filename,
					 const string& jisStd="jis208",
					 const string& encoding="euc-jp");
	int LoadKanjidic2(const string& filename);
	int LoadRadkfile(const string& filename, const string& encoding="euc-jp");
	int LoadKradfile(const string& filename, const string& encoding="euc-jp");

	/* Kanjidic-specific stuff */
	void KanjidicToKInfo(const string& kanjidicEntry, KInfo& k,
						 const char* jisStd);
	void KanjidicParser(char* kanjidicRawData, const char* jisStd);
	static wstring ConvertKanjidicEntry(const wstring& s);

	/* Data */
	static KDict *kdictSingleton;
	unordered_map<wchar_t, KInfo> kdictData;
	unordered_map<wchar_t, wstring> radkData;
	unordered_map<wchar_t, int> radkDataStrokes;
};

#endif
