/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: file_utils.cpp

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Include platform-specific files here */
#ifdef __WXMSW__
#include <windows.h>
#else
#include <unistd.h>
#endif

/* Normal includes go here */
#include "file_utils.h"
#include "encoding_convert.h"
#include <fstream>
#include <sstream>
using namespace std;

/* Tries to open the specified file.  If successful, it'll read in its entire
   contents into a wstring, and apply the specified conversion. */
int ReadEncodedFile(const char *filename, wstring& dest,
					const char *src_encoding) {
	ifstream ifile(filename, ios::in | ios::binary);
	stringbuf data;
	if(ifile.is_open()) {
		while(!ifile.eof() && !ifile.fail())
			ifile >> &data;
		ifile.close();
		if(ifile.fail()) return REF_FILE_OPEN_ERROR;

		dest = ConvertString<char, wchar_t>
			(data.str().c_str(), src_encoding, wcType.c_str());
		return REF_SUCCESS;

	} else return REF_FILE_NOT_FOUND;
}

/* Platform-independent getcwd function. */
string GetCWD() {
	string s;

	/* Yes, the size for the buffer used in
	   this function is maybe overkill. */
#ifdef __WXMSW__
	wchar_t *buffer = new wchar_t[0x10000];
	GetCurrentDirectory(0x10000, buffer);
	s = ConvertString<char>((char *)buffer, wcType.c_str(), "UTF-8");
#else
	char *buffer = new char[0x10000];
	if(getcwd(buffer, 0x10000))
		s = buffer;
#endif

	delete[] buffer;
	return s;
}
