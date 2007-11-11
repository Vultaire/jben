#ifndef string_utils_h
#define string_utils_h

#include <string>
using namespace std;

string StrToLower(const string& original);
bool GetIndexAfterParens(const string& s, size_t indexParen, size_t& indexNext,
						 char cOpenParen='(', char cCloseParen=')');
bool GetIndexBeforeParens(const string& s, size_t indexParen, size_t& indexNext,
						  char cOpenParen='(', char cCloseParen=')');

#endif
