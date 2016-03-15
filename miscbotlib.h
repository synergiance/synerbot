/*
 * miscbotlib.h
 *
 *  Created on 19 February 2014
 *      By: Synergiance
 */

/* Regex version define:
 * POSIX_REGEX - Use the standard posix regex (not quite working)
 * BOOST_REGEX - Use Boost's regex implementation (Check for boost support)
 * STD_REGEX - Use only if you have a newer compiler
 */

#define STD_REGEX

#include <string>
#include <vector>

using namespace std;

string toLower(string str);
string toUpper(string str);

void trimWhite(string& text);

string makeBorder(string title);

bool getFirstWord(string message, string& first, string& rest);

bool matchesChars(char chr, string str);

bool parseUser(string fqdn, string& nick, string& user, string& host);

void compare(string str1, string str2, unsigned& begin, unsigned& end);

// Regex forwarders
bool rgxMatch(string str1, string str2);
bool rgxMatch(string str1, string str2, vector<string> &groups);
bool rgxSearch(string str1, string str2);
bool rgxSearch(string str1, string str2, string &str3);
bool rgxSearch(string str1, string str2, vector< vector<string> > &matches);
string rgxReplace(string str1, string str2, string str3);
string rgxReplaceGroup(string str1, string str2, string str3);

// Unicode
long fromUnicode(string str);
string toUnicode(long val);
size_t unicodeLen(char c);
vector<long> unidecode(string str);
