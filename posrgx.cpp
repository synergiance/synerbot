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

using std::string;

bool rgxPos(string str1, string str2, size_t & start, size_t & finish)
{// Gets the locations on the first match
    regex_t    rgxCmp;
    regmatch_t rgxGrp[1]; // 1 length array returns only the overall match

    if (regcomp(&rgxCmp, str2.c_str(), REG_EXTENDED)) return false;
    if (regexec(&rgxCmp, str1.c_str(), 1, rgxGrp, 0)) return false;

    start  = rgxGrp[0].rm_so;
    finish = rgxGrp[0].rm_eo;

    return true;
}

bool posrgx_match(string str1, string str2)
{// Determines if entire string matches a regex
    size_t s, f;
    if (rgxPos(str1, str2, s, f))
        if (s == 0 && f == str1.length())
            return true;
    return false;
}

bool posrgx_search(string str1, string str2)
{// Looks for a single match in a regex
    size_t a, b; return rgxPos(str1, str2, a, b);
}

string  posrgx_return(string str1, string str2)
{// Returns the first match in string format
    size_t s, f;
    if (rgxPos(str1, str2, s, f))
        return str1.substr(s, f - s);
    return string();
}

string posrgx_replace(string str1, string str2, string str3)
{// Replaces all matches with specified string
    return string();
}
