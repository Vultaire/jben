/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: jutils.h

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

#ifndef jutils_h
#define jutils_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <string>
using namespace std;

extern wxCSConv transcoder;

bool IsFurigana(wxChar c);
bool IsHiragana(wxChar c);
bool IsKatakana(wxChar c);
bool UTF8ToWx(const string& utfStr, wxString& wxStr);
bool WxToUTF8(const wxString& wxStr, string& utfStr);

/* int AddKanjiFromFile(wxWindow *owner); */

#endif
