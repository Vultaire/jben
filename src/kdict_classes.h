/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: kdict_classes.h

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

#ifndef kdict_classes_h
#define kdict_classes_h

#include <string>
#include <list>
using namespace std;

class SkipCode {
public:
	SkipCode();
	SkipCode(const string& skipStr);
	string str() const;
	bool is_set() const;

	int i1, i2, i3;
};

/* New KInfo.  Structure is less flexible but should be faster, especially on
   load time.  Lists remain but maps have been flattened into the class. */
class KInfo {
public:
	KInfo();

	string literal;
	string cp_j208, cp_j212, cp_j213, cp_ucs;
	unsigned char radical, radicalNelson;
	int grade;
	int strokeCount;
	list<int> misstrokes;
	string var_j208, var_j212, var_j213, var_ucs,
		var_deroo, var_nelson_c, var_njecd, var_oneill, var_s_h;
	int freq;
	string radicalName;
	string dc_busy_people, dc_crowley, dc_gakken, dc_halpern_kkld,
		dc_halpern_njecd, dc_heisig, dc_henshall, dc_henshall3, dc_jf_cards,
		dc_kanji_in_context, dc_kodansha_compact, dc_moro, dc_nelson_c,
		dc_nelson_n, dc_oneill_kk, dc_oneill_names, dc_sakade, dc_sh_kk,
		dc_tutt_cards;
	string qc_deroo, qc_four_corner, qc_sh_desc;
	SkipCode qc_skip;
	list< pair<string, SkipCode> > skipMisclass;  /* Maybe this should be a
												  std::multimap instead? */
	list<string> pinyin, korean_r, korean_h,
		onyomi, kunyomi, nanori;
	/* FOR NOW: I am ignoring onyomi and kunyomi r_status and on_type flags.
	   In the KANJIDIC2 dated 2008-02-12, there was only one entry, "行く",
	   which had any of these flags.  Not worth the effort. */
	list<string> meaning, meaning_es, meaning_pt;
	wstring kradData;
};

#endif
