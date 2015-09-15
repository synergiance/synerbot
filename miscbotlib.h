/*
 * miscbotlib.h
 *
 *  Created on 19 February 2014
 *      By: Tim Gerety
 */

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
bool rgxSearch(string str1, string str2);
string rgxReturn(string str1, string str2);
string rgxReplace(string str1, string str2, string str3);

// Unicode
long fromUnicode(string str);
string toUnicode(long val);
size_t unicodeLen(char c);
vector<long> unidecode(string str);
