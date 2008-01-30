/*
Project: J-Ben
Author:  Paul Goins
Website: http://www.vultaire.net/software/jben/
License: GNU General Public License (GPL) version 2
         (http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

File: wdict.cpp

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

#include "wdict.h"
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

/* SEARCH_MAX is our hard-coded cutoff point for searches.  It should be high
   enough not to interfere with normal "single page" operation, but it should
   also prevent the user from doing something too stupid and having to wait a
   minute or so because they searched for the letter "e" by mistake.

   The most commonly used kanji in EDICT2 appears to be 人, at 1889 characters.
   Thus, let's make our panic breakoff point at 2000 characters. */
#define SEARCH_MAX 2000

WDict* WDict::wdictSingleton = NULL;

const WDict *WDict::GetWDict() {
	if(!wdictSingleton)
		wdictSingleton = new WDict;
	return wdictSingleton;
}

WDict::WDict() {
	LoadEdict2();
}

void WDict::Destroy() {
	if(wdictSingleton) {
		delete wdictSingleton;
		wdictSingleton = NULL;
	}
}

int WDict::LoadEdict2(const char *filename) {
	WDict *e=NULL;
	char *rawData = NULL;
	unsigned int size;
	int returnCode = 0xDEADBEEF;

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
		this->Edict2Parser(rawData);

		returnCode = ED_SUCCESS;
	}
	else
		returnCode = ED_FAILURE;

	if(rawData) delete[] rawData;
	return returnCode;
}

/* EDICT2 parser for WDict.  Takes a wxString containing the contents of
   an EDICT- or EDICT2-formatted dictionary, and adds its contents to an
   internal data struct.  This function also indexes the data, although ideally
   the indexing functionality should be externalized so it may be called later,
   like if another dictionary is added into the same WDict object at a later
   point. */
void WDict::Edict2Parser(char *edictRawData) {
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

WDict::~WDict() {
	/* Currently, nothing needs to be done here. */
}

bool WDict::Search(const wxString& query, list<int>& results,
				   unsigned int searchType) const {
	list<int> priorityResults[4];
	bool englishSearch;
	bool isFurigana;  /* Not sure this is necessary - currently set but not
						 used.  May be a performance accelerator at cost of
						 code complexity. */
	int priorityExact, priorityBeginsWith, priorityEndsWith, priorityOther;
	vector<string>::const_iterator vIt;

	if(query.length()==0) {
#ifdef DEBUG
		printf("[%s:%d] Empty string passed into WDict::Search.  (Not a problem!)\n", __FILE__, __LINE__);
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

	/* Main search code begins below */
	/* NOTE: I think this can be cleaned up.  I don't think the vector for
	   entryData below is needed; a simple string should suffice and a loop can
	   be removed.  I'll look at this later since I'm busy with something else
	   at the moment. */

	vector<string> entryData; /* Stores the English/Japanese components of
								 an EDICT string. */
	string utfQuery, lwrQuery, lwrData;
	vector<string>::iterator vSubIt;
	size_t indexSubstr, indexDataStart, indexDataEnd;
	int priorityLevel;
	char c;

	WxToUTF8(query, utfQuery);
	lwrQuery = StrToLower(utfQuery); /* For English searching, store a
										lowercase query */
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
				/* English searching requires 2 special conditions:
				   1. Case-insensitive searching (maybe optional, later)
				   2. Recognition of word bounds (so we don't match character
				      sequences inside of a word.) */

				/* Convert target string to lower case */
				lwrData = StrToLower(*vSubIt);

				/* Find the first match that is bounded by non-alpha characters
				   or beginning/end of string. */
				indexSubstr = lwrData.find(lwrQuery, 0);
				while(indexSubstr!=string::npos) {
#ifdef DEBUG
					printf("Checking possible match:\n"
						   "Query:       [%s]\n"
						   "Data string: [%s]\n"
						   "Index of match: %d\n",
						   lwrQuery.c_str(), lwrData.c_str(), indexSubstr);
#endif
					if(
						/* Check for beginning of data string or preceding
						   non-alpha char */
						(indexSubstr==0 || !isalpha(lwrData[indexSubstr-1])) &&
						/* Check for end of data string or following non-alpha
						   char */
						(indexSubstr+lwrQuery.length() == lwrData.length() ||
						 !isalpha(lwrData[indexSubstr+lwrQuery.length()]))
						) break;
					/* If the match didn't meet all the above criteria, try to
					   find the next one. */
#ifdef DEBUG
					printf("Match not good.  Displaying verbose data:\n");
					if(indexSubstr==0)
						printf("* Beginning of query matches beginning of data. (OK)\n");
					else if(!isalpha(lwrData[indexSubstr-1]))
						printf("* Preceding character '%c' is non-alpha. (OK)\n",
							   lwrData[indexSubstr-1]);
					else
						printf("* Start match is invalid. (FAIL)\n");
					if(indexSubstr+lwrQuery.length() == lwrData.length())
						printf("* End of query matches end of data. (OK)\n");
					else if(!isalpha(lwrData[indexSubstr+lwrQuery.length()]))
						printf("* Following character '%c' is non-alpha. (OK)\n",
							   lwrData[indexSubstr+lwrQuery.length()]);
					else
						printf("* End match is invalid. (FAIL)\n");
						
#endif
					indexSubstr = lwrData.find(lwrQuery, indexSubstr+1);
				}
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
					} else {
					}
				} else if(indexSubstr == indexDataEnd+1 - utfQuery.length()) {
					priorityLevel = max(priorityLevel, priorityEndsWith);
				} else {
					priorityLevel = max(priorityLevel, priorityOther);
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
			} else {
#ifdef DEBUG
				printf("PANIC: SEARCH_MAX results reached!\n");
#endif
				wxMessageBox(wxString::Format(_T("Over %d results were found.  The search has been stopped."), SEARCH_MAX),
							 _T("Excessive search results"),
							 wxOK | wxICON_INFORMATION, NULL);
				break;
			}
		}

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

#ifdef DEBUG
	printf("Search result count: %d\n", results.size());
#endif
	if(results.size()>0) return true;
	return false;
}

wxString WDict::ResultToHTML(const wxString& rawResult) {
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

void WDict::GetEnglish(const string& edictStr, vector<string>& dest) {
	char *tokenizedString = new char[edictStr.length()+1];
	char *token;

	strcpy(tokenizedString, edictStr.c_str());
	token = strtok(tokenizedString, "/");
	/* Skip to the second token, since the first is just the Japanese readings */
	if(token) {
		token = strtok(NULL, "/");
	}

	while(token) {
		if(strlen(token)>0) dest.push_back(token);
		token = strtok(NULL, "/");
	}

	delete[] tokenizedString;
}

void WDict::GetJapanese(const string& edictStr, vector<string>& dest) {
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

string WDict::GetEdictString(int i) const { return edictData[i]; }

bool WDict::MainDataLoaded() const {
	if(edictData.size()>0) return true;
	return false;
}
