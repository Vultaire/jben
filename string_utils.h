/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: string_utils.h

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

#ifndef string_utils_h
#define string_utils_h

#include <string>
#include <list>
using namespace std;

string StrToLower(const string& src);
bool GetIndexAfterParens(const string& s, size_t indexParen, size_t& indexNext,
						 char cOpenParen='(', char cCloseParen=')');
bool GetIndexBeforeParens(const string& s, size_t indexParen, size_t& indexNext,
						  char cOpenParen='(', char cCloseParen=')');
string ToUpper(string src);
wstring ToUpper(wstring src);
string ToLower(string src);
wstring ToLower(wstring src);
string Trim(string);
wstring Trim(wstring);

/* Template functions */
template <class t>
list< basic_string<t> > StrTokenize
(const basic_string<t>& src,
 const t *delimiters,
 bool emptyTokens = false) {
	list< basic_string<t> > l;
	size_t start = 0;
	size_t end = src.find(delimiters, start);

	while(end!=basic_string<t>::npos) {
		if(emptyTokens || (start!=end)) {
			l.push_back(
				src.substr(start, end-start));
		}
		start = end + 1;
		end = src.find(delimiters, start);
	}
	if(start!=basic_string<t>::npos) {
		basic_string<t> lastStr = src.substr(start);
		if(lastStr.length()>0) l.push_back(lastStr);
	}
	return l;
}

#endif
