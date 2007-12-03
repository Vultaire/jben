/*
Project: J-Ben
Author:  Paul Goins
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: edict.cpp

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

#include "edict.h"
#include "file_utils.h"
#include "wx/tokenzr.h"
#include "jutils.h"
#include "string_utils.h"
#include <set>
#include <list>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>
using namespace std;

#define SEARCH_MAX 250

Edict *Edict::LoadEdict(const char *filename, int& returnCode) {
	Edict *e=NULL;
	char *rawData = NULL;
	unsigned int size;

	ifstream ifile(filename, ios::ate); /* "at end" to get our file size */
	if(ifile) {
		size = ifile.tellg();
		ifile.seekg(0);
		rawData = new char[size+1];
		rawData[size] = '\0';
		ifile.read(rawData, size);
#ifdef DEBUG
		if(strlen(rawData)!=size)
			fprintf(stderr,
			  "WARNING: edict file size: %d, read-in string: %d\n",
			  strlen(rawData),
			  size);
#endif

		/* Create the kanjidic object with our string data. */
		e = new Edict(rawData);

#ifdef DEBUG
		printf("Edict created. (%p)\n", e);
#endif
		returnCode = ED_SUCCESS;
	}
	else
		returnCode = ED_FAILURE;

	if(rawData) delete[] rawData;
	return e;
}

/* Default constructor for Edict.  Takes a wxString containing the contents of
   an EDICT- or EDICT2-formatted dictionary, and adds its contents to an
   internal data struct.  This function also indexes the data, although ideally
   the indexing functionality should be externalized so it may be called later,
   like if another dictionary is added into the same Edict object at a later
   point. */
Edict::Edict(char *edictRawData) {
	char *token;
	wxString wxToken;

	int vIndex = -1; /* edict vector index */
	wxString sTemp;

	/* Store raw EDICT data, plus store references by kanji/reading into ordered
	   set */
	token = strtok(edictRawData, "\n");
	while(token) {
		if(strlen(token)>0) {
			/* 0. Make wxString copy of the token */
			UTF8ToWx(token, wxToken);
			/* 1. Store full string in vector */
			edictData.push_back(token);
			vIndex++;

		}
		token = strtok(NULL, "\n");
	} /* while has more tokens */
}

Edict::~Edict() {
#ifdef DEBUG
	printf("Edict destroyed. (%p)\n", this);
#endif
}

/* This function walks through the string, watching the parentheses, and copying
   only the portions which are outside parentheses.  Nested parentheses are
   handled. */
wxString Edict::StripParenFields(const wxString& src) {
	wxString result;
	int parenCount = 0;
	size_t length, index, lastIndex, startValid;

	startValid = 0;
	length = src.length();
	index = src.find_first_of(_T("()"));
	while(index != wxString::npos) {
		if(src[index]==_T('(')) {
			parenCount++;
			if(parenCount==1) {
				/* Append the valid string up until parens were entered. */
				result.append(src.substr(startValid, index - startValid));
			}
		} else {
			parenCount--;
			if(parenCount==0) {
				/* Parens have been exited.  Reset our valid index. */
				startValid = index+1;
			}
			if(parenCount<0) parenCount=0; /* We'll skip extra )'s */
		}
		lastIndex = index;
		index = src.find_first_of(_T("()"), lastIndex+1);
	}
	if(parenCount>0) {
#ifdef DEBUG
		fprintf(stderr, "WARNING: %s:%d, StripParenFields: Unclosed '(' detected.\n\tString: %ls\n",
			__FILE__, __LINE__, src.c_str());
#endif
	} else {
		/* Append any remainder of the original string */
		if(startValid!=wxString::npos && startValid < length) {
			result.append(src.substr(startValid));
		}
	}

#if 0
#ifdef DEBUG
	printf("StripParenFields:\n\tOrig:[%ls]\n\tNew:[%ls]\n",
		src.c_str(), result.c_str());
#endif
#endif
	return result;
}

bool Edict::Search(const wxString& query, list<int>& results,
				   unsigned int searchType) {
	list<int> priorityResults[4];
	bool englishSearch;
	bool isFurigana;  /* Not sure this is necessary - currently set but not
						 used.  May be a performance accelerator at cost of
						 code complexity. */
	int priorityExact, priorityBeginsWith, priorityEndsWith, priorityOther;
	vector<string>::iterator vIt;

	if(query.length()==0) {
#ifdef DEBUG
		printf("[%s:%d] Empty string passed into Edict::Search.  (Not a problem!)\n", __FILE__, __LINE__);
#endif
		return false;
	}

	/* Get our search priorities set up */
	int i;
	unsigned int uTemp;

	/* Default priority is -1, "not used" */
	/* Lowest priority is 0, and highest will be 3. */
	/* HOWEVER, searchType is sorted as 0:7=high priority and 24:31=low. */
	priorityExact = priorityBeginsWith = priorityEndsWith = priorityOther = -1;
	for(i=0;i<4;i++) {
		uTemp = (searchType >> ((3-i)*8)) & 0xFF;
		if(uTemp == EDS_EXACT) priorityExact = i;
		else if(uTemp == EDS_BEGIN) priorityBeginsWith = i;
		else if(uTemp == EDS_END) priorityEndsWith = i;
		else if(uTemp == EDS_ANY) priorityOther = i;
		else if(uTemp == 0) { /* Do nothing; no preferred search method for
								 this level */ }
		else {
#ifdef DEBUG
			fprintf(stderr, "Unknown search type for priority level %d: %X\n", i+1, uTemp);
#endif
		}
	}

	/* Store first char.  This determines whether we're doing an E-J or J-E
	   search. */
	wxChar firstChar = query[0];
	/* Using a very, very simple check: is it just a 7-bit char? */
	englishSearch = ( ((unsigned)firstChar) <= 0x7F );
	if(!englishSearch) {
		isFurigana=true;
		for(wxString::const_iterator stringIt = query.begin();
		  stringIt!=query.end(); stringIt++) {
			isFurigana = (IsFurigana(*stringIt));
			if(!isFurigana) break;
		}
	}

	/* NEW search code: non-accelerated search using only the data in EDICT2,
	   no extra memory-consuming indices.  BESIDES, if we do more than a
	   "begins with" or "exact match" search, we have to do a non-indexed
	   search to catch the other stuff anyway... so let's just do it all at the
	   same time. */

	vector<string> entryData;
	string utfQuery;
	vector<string>::iterator vSubIt;
	size_t indexSubstr, indexDataStart, indexDataEnd;
	int priorityLevel;
	char c;

	WxToUTF8(query, utfQuery);
	i = 0;
	for(vIt=edictData.begin(); vIt!=edictData.end(); vIt++) {
		priorityLevel = -1; /* -1 == not a match*/
		if(englishSearch) {
			GetEnglish(*vIt, entryData);
		} else {
			GetJapanese(*vIt, entryData);
		}
		
		for(vSubIt=entryData.begin(); vSubIt!=entryData.end(); vSubIt++) {
			if(englishSearch) {
				/* Special handling needed for case-insensitive English
				   searching. */
				string lwrData = StrToLower(*vSubIt);
				string lwrQuery = StrToLower(utfQuery);
				indexSubstr = lwrData.find(lwrQuery, 0);
			} else {
				indexSubstr = vSubIt->find(utfQuery, 0);
			}
			if(indexSubstr!=string::npos) {
				/* A match was found.
				   Sort by type of match (exact, begin, end, other)
				   LOGIC:
				   - Search for a "begins with".
				   - If it matches, check for "exact" (string length will work).
				   - Check for an "ends with" (parens may be a prob??)
				   - Dump all others into "other" */

				/* FIRST: We need to get our dictionary bounds.  Check for an
				   opening parenthesis, and if present, skip past it. */
				indexDataStart = 0;
				c = (*vSubIt)[indexDataStart];
				if(c == '(' || c == '{') {
					/* Parens found.  Loop until we reach the beginning of the
					   real data. */
					while(1) {
						/* Get first non-space char past the end parenthesis. */
						if(c=='(') {
							if(GetIndexAfterParens(*vSubIt, indexDataStart,
												   indexDataStart, '(', ')')) {
								while(isspace((*vSubIt)[indexDataStart]))
									indexDataStart++;
							}
						} else if(c=='{') {
							if(GetIndexAfterParens(*vSubIt, indexDataStart,
												   indexDataStart, '{', '}')) {
								while(isspace((*vSubIt)[indexDataStart]))
									indexDataStart++;
							}
						} else break;
						c = (*vSubIt)[indexDataStart];
					}
				}

				/* Get the ending bound.
				   NOTE: Currently this is only done for Japanese entries.
				   English entries ending with ()'s will not omit them.  This is
				   deliberate. */
				indexDataEnd = vSubIt->length()-1;
				if(!englishSearch) {
					c = (*vSubIt)[indexDataEnd];
					if(c == ')' || c == '}') {
						/* Parens found.  Loop until we reach the beginning of
						   the real data. */
						while(1) {
							/* Get first non-space char past the end
							   parenthesis. */
							if(c==')') {
								if(GetIndexBeforeParens(*vSubIt, indexDataEnd,
														indexDataEnd, '(', ')')) {
									while(isspace((*vSubIt)[indexDataEnd]))
										indexDataEnd--;
								}
							} else if(c=='}') {
								if(GetIndexBeforeParens(*vSubIt, indexDataEnd,
														indexDataEnd, '{', '}')) {
									while(isspace((*vSubIt)[indexDataEnd]))
										indexDataEnd--;
								}
							} else break;
							c = (*vSubIt)[indexDataEnd];
						}
					}
				}

				/* Now, we apply the logic we specified at the beginning of this
				   block. */
				if(indexSubstr==indexDataStart) {
					priorityLevel = max(priorityLevel, priorityBeginsWith);
					if(utfQuery.length()==indexDataEnd+1 - indexDataStart) {
						priorityLevel = max(priorityLevel, priorityExact);
#ifdef DEBUG
						printf("MATCH! Type: EXACT: (%s = %s)\n", utfQuery.c_str(),
							   vSubIt->c_str());
#endif
					} else {
#ifdef DEBUG
						printf("MATCH! Type: Begins With (%s = %s)\n", utfQuery.c_str(),
							   vSubIt->c_str());
#endif
					}
				} else if(indexSubstr == indexDataEnd+1 - utfQuery.length()) {
					priorityLevel = max(priorityLevel, priorityEndsWith);
#ifdef DEBUG
						printf("MATCH! Type: Ends With (%s = %s)\n", utfQuery.c_str(),
							   vSubIt->c_str());
#endif
				} else {
					priorityLevel = max(priorityLevel, priorityOther);
#ifdef DEBUG
						printf("MATCH! Type: Other (%s = %s)\nDS=%d, DE=%d\n",
							   utfQuery.c_str(),
							   vSubIt->substr(indexDataStart,
											  indexDataEnd+1-indexDataStart).c_str(),
							   indexDataStart, indexDataEnd);
#endif
				}
			}
		}
		/* Add to appropriate list */
		if(priorityLevel>=0) {
			if(priorityResults[0].size()
			  +priorityResults[1].size()
			  +priorityResults[2].size()
			  +priorityResults[3].size()< SEARCH_MAX) {
				priorityResults[priorityLevel].push_back(i);
			} else break;
		}

#if 0
		if(entryData.size()>0) {
			printf("Entry %d: Substrings found:\n", i);
			for(vSubIt=entryData.begin(); vSubIt!=entryData.end(); vSubIt++)
				printf("\t[%s]\n", vSubIt->c_str());
		} else {
			printf("Entry %d: no substrings found.\n", i);
		}
#endif

		entryData.clear();
		i++;
	}

	/* Combine results into one list, based upon priority. */
	list<int>::iterator lIt;
	for(i=3;i>=0;i--) {
		for(lIt=priorityResults[i].begin();
		  lIt!=priorityResults[i].end(); lIt++) {
			results.push_back(*lIt);
		}
	}

	if(results.size()>0) return true;
	return false;
}

wxString Edict::ResultToHTML(const wxString& rawResult) {
	wxString token, subToken, jStr, eStr, htmlStr;
	wxStringTokenizer tk(rawResult, _T("\n"));
	size_t indexSlash, indexNextSlash, indexBreak;
	while(tk.HasMoreTokens()) {
		token = tk.GetNextToken();
		htmlStr.append(_T("<p>"));

		indexSlash = token.find_first_of(_T('/'));
		if(indexSlash==wxString::npos) {
			/* Fail-safe: just display the raw string */
			htmlStr.append(token);
		} else {
			htmlStr.append(_T("<b>Japanese:</b> <font size=\"6\">"));
			/*htmlStr.append(token.substr(0,indexSlash));*/
			jStr = token.substr(0,indexSlash);

			indexBreak = jStr.find_first_of(_T(';'));
			while(indexBreak!=wxString::npos) {
				/*jStr[indexBreak]=_T(", ");*/
				jStr.replace(indexBreak,1,_T(", "),0,2);
				indexBreak = jStr.find_first_of(_T(';'));
			}
			
			htmlStr.append(jStr);
			htmlStr.append(_T("</font><br>"));

			htmlStr.append(_T("<b>English:</b> "));
			eStr.clear();
			while(indexSlash!=wxString::npos) {
				indexNextSlash = token.find_first_of(_T('/'), indexSlash+1);
				if(indexNextSlash==wxString::npos)
					subToken = token.substr(indexSlash+1);
				else
					subToken = token.substr(indexSlash+1,
											indexNextSlash-1 - indexSlash);
				if(subToken.length()>0) {
					if(eStr.length()>0)
						eStr.append(_T("; "));
					eStr.append(subToken);
				}
				indexSlash = indexNextSlash;
			}
			htmlStr.append(eStr);
		}
		
		htmlStr.append(_T("</p>"));
	}

	return htmlStr;
}

void Edict::GetEnglish(const string& edictStr, vector<string>& dest) {
	char *tokenizedString = new char[edictStr.length()+1];
	char *token;

	/*printf("DEBUG: GetEnglish(%s)\n", edictStr.c_str());*/
	strcpy(tokenizedString, edictStr.c_str());
	token = strtok(tokenizedString, "/");
	/* Skip to the second token, since the first is just the Japanese readings */
	if(token) {
		/*printf("First token: [%s]\n", token);*/
		token = strtok(NULL, "/");
	}
	
	while(token) {
		/*printf("Next token found: [%s]\n", token);*/
		if(strlen(token)>0) dest.push_back(token);
		token = strtok(NULL, "/");
	}

	delete[] tokenizedString;
}

/* Rewrite this code to handle the different tokenization of the Japanese part
   of the string. */
void Edict::GetJapanese(const string& edictStr, vector<string>& dest) {
	/* Grab the portion of the string relevant for Japanese readings */
	size_t indexFinal = edictStr.find_first_of('/');
	if(indexFinal==string::npos) indexFinal = edictStr.length();
	string jStr = edictStr.substr(0, indexFinal);
	string temp;

	/* The data is too complex for a simple tokenization because strings within
	   parentheses may contain characters normally used for breaking up the
	   tokens.  So, the logic here is a little more complex. */
	size_t index, indexBreak, indexParen, indexStart=0;
	size_t len=jStr.length();

	index = indexStart;
	while(indexStart<len) {
		while(true) {
			indexBreak = jStr.find_first_of(";[] ", index);
			indexParen = jStr.find_first_of('(', index);

			/* Valid String Breaks */
			/* If no parentheses are found, then indexBreak indicates our
			   bounds properly. */
			if(indexParen==string::npos) break;
			/* If parentheses ARE found, then we want to process them...
			   UNLESS a break char is found before the parenthesis. */
			if(indexBreak!=string::npos && indexBreak<indexParen) break;

			/* Skip the parentheses and set index equal to the index following
			   the ')' character. */
			if(!GetIndexAfterParens(jStr, indexParen, index)) {
				indexBreak = string::npos;
				break;
			}
		}

		if(indexBreak==string::npos) {
			temp = jStr.substr(indexStart);
		} else {
			temp = jStr.substr(indexStart, indexBreak-indexStart);
		}
		if(temp.length()>0) dest.push_back(temp);

		/* Return if either indexBreak or index == string::npos.
		   index==string::npos:
		     This happens either if indexStart == string::npos, or if
			 parsing parentheses and we can't find a closing parenthesis.
		   indexBreak==string::npos:
		     This happens if the substring continues to the end of the source
			 string. */
		if(index==string::npos
	  	|| indexBreak==string::npos) {
			return;
		}

		/* Iterate relevant vars for next iteration */
		indexStart = indexBreak+1;
		index = indexStart;
	}
}

string Edict::GetEdictString(int i) { return edictData[i]; }
