/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: jutils.h

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
bool EUCToWx(const string& eucStr, wxString& wxStr);
bool WxToEUC(const wxString& wxStr, string& eucStr);

/* int AddKanjiFromFile(wxWindow *owner); */

#endif
