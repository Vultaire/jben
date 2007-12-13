/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: jutils.cpp

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

#include "jutils.h"

wxCSConv transcoder(_T("utf8"));

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

bool UTF8ToWx(const string& utfStr, wxString& wxStr) {
	int srcLength = utfStr.length();
	int destLength = (srcLength+1) * sizeof(wxChar);

	transcoder.MB2WC(
	  wxStr.GetWriteBuf(destLength),
	  utfStr.c_str(), destLength);
	wxStr.UngetWriteBuf();

	/* This will be a performance hit if used, I think.  But I'll leave it
	   in as a comment for possible future memory savings. */
	/*wxStr.Shrink();*/
	return true;
}

bool WxToUTF8(const wxString& wxStr, string& utfStr) {
	/* UTF8 can take up to 4 bytes for each single UNICODE character.
	   So, instead of sizeof(wxChar) or similar, we just use a hard-coded
	   "3". */
	int srcLength = wxStr.length();
	int destLength = (srcLength+1) * 4;
	char *temp = new char[destLength];

	transcoder.WC2MB(temp, wxStr.c_str(), destLength);

	utfStr = temp;
	delete[] temp;
	return true;
}
