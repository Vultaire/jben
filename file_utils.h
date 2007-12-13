/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: file_utils.h

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

#ifndef file_utils_h
#define file_utils_h

/* Stock wxWidgets includes */
#include "wx/wxprec.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#define REF_SUCCESS            0x0
#define REF_FAILURE            0x80000000
#define REF_FILE_NOT_FOUND     REF_FAILURE | 0x1
#define REF_FILE_OPEN_ERROR    REF_FAILURE | 0x2

int ReadEncodedFile(const wxChar *filename, const wxChar *src_encoding, wxString& dest);
int ReadFile(const wxChar *filename, wxString& dest);

#endif
