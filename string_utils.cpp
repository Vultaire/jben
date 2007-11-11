#include "string_utils.h"

string StrToLower(const string& original) {
	string result = original;
	for(string::iterator it=result.begin(); it!=result.end(); it++) {
		*it = tolower(*it);
	}
	return result;
}

bool GetIndexAfterParens(const string& s, size_t indexParen, size_t& indexNext,
						 char cOpenParen, char cCloseParen) {
	int parenCount = 1;
	size_t index = indexParen+1;
	string sParenChars("  ");
	sParenChars[0]=cOpenParen;
	sParenChars[1]=cCloseParen;

	while(parenCount>0) {
		/* Skip "index" ahead to after the closing parenthesis, or to the
		   end if not found. */
		index = s.find_first_of(sParenChars, index);
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
