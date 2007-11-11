/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: jutils.cpp

This file is part of J-Ben.

J-Ben is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

J-Ben is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "jutils.h"

wxCSConv transcoder(_T("euc-jp"));

bool IsFurigana(wxChar c) {
	if(IsHiragana(c) || IsKatakana(c)) return true;
	return false;
}

bool IsHiragana(wxChar c) {
	int i = (int) c;
	if(i >= 0x3041 && i <= 0x3096) return true;
	return false;
}

bool IsKatakana(wxChar c) {
	int i = (int) c;
	if(i >= 0x30A1 && i <= 0x30FA) return true;
	return false;
}

bool EUCToWx(const string& eucStr, wxString& wxStr) {
	int srcLength = eucStr.length();
	int destLength = (srcLength+1) * sizeof(wxChar);

/* OLD (DEPRECATED) CODE */
#if 1
	transcoder.MB2WC(
	  wxStr.GetWriteBuf(destLength),
	  eucStr.c_str(), destLength);
	wxStr.UngetWriteBuf();
#endif
/* NEW CODE */
/* Disabled: Documentation seems TOTALLY counter-intuitive.  It says that
   "ToWChar" converts a wide string to a multibyte... or by my understanding
   of the English language, that it converts FROM a wide character string TO
   a multi-byte one.  Totally backwards.   Therefore, disabled; I'll use
   the function which is more clearly documented. */
#if 0
	transcoder.ToWChar(
	  wxStr.GetWriteBuf(destLength),
	  destLength,
	  eucStr.c_str());
	wxStr.UngetWriteBuf();
#endif

	/* This will be a performance hit if used, I think.  But I'll leave it
	   in as a comment for possible future memory savings. */
	/*wxStr.Shrink();*/
	return true;
}

bool WxToEUC(const wxString& wxStr, string& eucStr) {
	/* EUC-JP can take up to 3 bytes for each single UNICODE character.
	   So, instead of sizeof(wxChar) or similar, we just use a hard-coded
	   "3". */
	int srcLength = wxStr.length();
	int destLength = (srcLength+1) * 3;
	char *temp = new char[destLength];
/* OLD (DEPRECATED) CODE */
#if 1
	transcoder.WC2MB(temp, wxStr.c_str(), destLength);
#endif
/* NEW CODE */
/* Reason for why this is disabled is mentioned one function above. */
#if 0
	transcoder.FromWChar(
	  temp,
	  destLength,
	  wxStr.c_str());
#endif
	eucStr = temp;
	delete[] temp;
	return true;
}
