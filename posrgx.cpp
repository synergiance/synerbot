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
#include <vector>

using std::string;
using std::vector;

bool rgxPos(string str1, string str2, regmatch_t &match)
{// Gets the locations on the first match
    regex_t    rgxCmp;
    regmatch_t rgxGrp[1]; // 1 length array returns only the overall match

    if (regcomp(&rgxCmp, str2.c_str(), REG_EXTENDED)) return false;
    if (regexec(&rgxCmp, str1.c_str(), 1, rgxGrp, 0))
    { regfree(&rgxCmp); return false; }

    match = rgxGrp[0];
    regfree(&rgxCmp);

    return true;
}

bool rgxPos(string str1, string str2, vector< vector<regmatch_t> > &matches)
{// Gets the locations on the first match
    char       * source;
    char       * rgxPos;
    regex_t      rgxCmp; // Compiled regex
    regmatch_t * rgxGrp; // Return regex array
    size_t       rgxMax; // Maximum number of groups
    size_t       rgxNum; // Number of groups
    bool         retVal;

    if (regcomp(&rgxCmp, str2.c_str(), REG_EXTENDED)) return false;

    rgxMax = 10; // POSIX states a maximum of 9 groups
    rgxGrp = new regmatch_t[rgxMax];
    source = (char *)str1.c_str();
    rgxPos = source;
    retVal = false;

    matches.clear();

    for (;;) {
        if (regexec(&rgxCmp, rgxPos, rgxMax, rgxGrp, 0)) break;
        retVal = true;
        rgxNum = rgxCmp.re_nsub;
        if (rgxNum > rgxMax) {
            rgxMax = rgxNum;
            delete rgxGrp;
            rgxGrp = new regmatch_t[rgxMax];
            regexec(&rgxCmp, rgxPos, rgxMax, rgxGrp, 0);
        }
        vector<regmatch_t> groups;
        for (size_t c = 0; c < rgxNum; c++) {
            if ((size_t)rgxGrp[c].rm_so == (size_t)-1) break;
            groups.push_back(rgxGrp[c]);
        }
        rgxPos += rgxGrp[0].rm_eo;
        matches.push_back(groups);
    }

    delete   rgxGrp;
    regfree(&rgxCmp);

    return retVal;
}

bool posrgx_match(string str1, string str2)
{// Determines if entire string matches a regex
    regmatch_t r;
    if (rgxPos(str1, str2, r))
        if (r.rm_so == 0 && (size_t)r.rm_eo == str1.length())
            return true;
    return false;
}

bool posrgx_match(string str1, string str2, vector<string> &groups)
{// Returns true and fills a vector with the groups if the regex is matched
    vector< vector<regmatch_t> > matches;
    if (rgxPos(str1, str2, matches)) {
        groups.clear();
        for (regmatch_t r : matches[0]) {
            groups.push_back(str1.substr(r.rm_so, r.rm_eo - r.rm_so));
        }
        return true;
    }
    return false;
}

bool posrgx_search(string str1, string str2)
{// Looks for a single match in a regex
    regmatch_t r; return rgxPos(str1, str2, r);
}

bool posrgx_search(string str1, string str2, string &str3)
{// Returns the first match in string format
    regmatch_t r;
    if (rgxPos(str1, str2, r)) {
        str3 = str1.substr(r.rm_so, r.rm_eo - r.rm_so);
        return true;
    }
    return false;
}

bool posrgx_search(string str1, string str2, vector< vector<string> > &matches)
{// Returns all matches and groups from the regex
    vector< vector<regmatch_t> > m;
    if (rgxPos(str1, str2, m)) {
        matches.clear();
        for (vector<regmatch_t> g : m) {
            vector<string> groups;
            for (regmatch_t r : g)
                groups.push_back(str1.substr(r.rm_so, r.rm_eo - r.rm_so));
            matches.push_back(groups);
        }
        return true;
    }
    return false;
}

string posrgx_replace(string str1, string str2, string str3)
{// Replaces all matches with specified string
    return string();
}

string posrgx_replace_group(string str1, string str2, string str3)
{// Replaces all matches with specified string
    return string();
}
