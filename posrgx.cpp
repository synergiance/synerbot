/*
 *   Filename: posrgx.cpp
 *
 * Created on: 9 January 2016
 *     Author: Synergiance
 *
 * A compatible buffer between regex.h and c++ nicities, will act like <regex>
 */

#include <regex.h>
#include <string>

bool posrgx_match(string str1, string str2)
{// Determines if entire string matches a regex
	return false;
}

bool posrgx_search(string str1, string str2)
{// Looks for a single match in a regex
	return false;
}

string  posrgx_return(string str1, string str2)
{// Returns the first match in string format
	return string();
}

string posrgx_replace(string str1, string str2, string str3)
{// Replaces all matches with specified string
	return string();
}
