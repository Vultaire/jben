/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: string_utils.cpp

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

#include "string_utils.h"
#include <cctype>
#include <cwctype>
using namespace std;

bool GetIndexAfterParens(const string& s, size_t indexParen, size_t& indexNext,
						 char cOpenParen, char cCloseParen) {
	int parenCount = 1;
	size_t index = indexParen;
	string sParenChars("  ");
	sParenChars[0]=cOpenParen;
	sParenChars[1]=cCloseParen;

	while(parenCount>0) {
		/* Skip "index" ahead to after the closing parenthesis, or to the
		   end if not found. */
		index = s.find_first_of(sParenChars, index+1);
		if(index==string::npos) break;
		if(s.at(index)==cOpenParen)
			parenCount++;
		else {
			parenCount--;
			if(parenCount==0) break;
		}
	}

	/* If the end of the string is found before parenthesis parsing is
	   finished, then set the break position to the end of the string. */
	if(index==string::npos) return false;

	indexNext = index+1;
	return true;
}

bool GetIndexBeforeParens(const string& s, size_t indexParen, size_t& indexNext,
						 char cOpenParen, char cCloseParen) {
	int parenCount = 1;
	size_t index = indexParen-1;
	string sParenChars("  ");
	sParenChars[0]=cOpenParen;
	sParenChars[1]=cCloseParen;

	while(parenCount>0) {
		/* Skip "index" ahead to after the closing parenthesis, or to the
		   end if not found. */
		index = s.find_last_of(sParenChars, index);
		if(index==string::npos) break;
		if(s.at(index)==cCloseParen)
			parenCount++;
		else {
			parenCount--;
			if(parenCount==0) break;
		}
	}

	/* If the end of the string is found before parenthesis parsing is
	   finished, then set the break position to the end of the string. */
	if(index==string::npos) return false;

	indexNext = index-1;
	return true;
}

string ToUpper(string src) {
	for(string::iterator i = src.begin(); i != src.end(); i++) {
		*i = toupper(*i);
	}
	return src;
}

wstring ToUpper(wstring src) {
	for(wstring::iterator i = src.begin(); i != src.end(); i++) {
		*i = towupper(*i);
	}
	return src;
}

string ToLower(string src) {
	for(string::iterator i = src.begin(); i != src.end(); i++) {
		*i = tolower(*i);
	}
	return src;
}

wstring ToLower(wstring src) {
	for(wstring::iterator i = src.begin(); i != src.end(); i++) {
		*i = towlower(*i);
	}
	return src;
}

string Trim(string src) {
	int len = src.length();
	int begin, end;

	for(begin=0; begin<len; begin++) {
		if(!isspace(src[begin])) break;
	}
	if(begin==len) return "";

	for(end=len-1; end>=0; end--) {
		if(!isspace(src[end])) break;
	}

	return src.substr(begin, end+1 - begin);
}

wstring Trim(wstring src) {
	int len = src.length();
	int begin, end;

	for(begin=0; begin<len; begin++) {
		if(!iswspace(src[begin])) break;
	}
	if(begin==len) return L"";

	for(end=len-1; end>=0; end--) {
		if(!iswspace(src[end])) break;
	}

	return src.substr(begin, end+1 - begin);
}
