/*
 *   Filename: posrgx.h
 *
 * Created on: 9 January 2016
 *     Author: Synergiance
 *
 * A compatible buffer between regex.h and c++ nicities.  Provides an
 * alternative to the c++11 builtin and boost regex libraries, which do not
 * exist or are unworkable on some platforms.
 */

#include <string>
#include <vector>

using std::string;
using std::vector;

bool posrgx_match(string str1, string str2);
bool posrgx_match(string str1, string str2, vector<string> &groups);
bool posrgx_search(string str1, string str2);
bool posrgx_search(string str1, string str2, string &str3);
bool posrgx_search(string str1, string str2, vector< vector<string> > &matches);
string posrgx_replace(string str1, string str2, string str3);
string posrgx_replace_group(string str1, string str2, string str3);
