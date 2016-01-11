/*
 * miscbotlib.cpp
 *
 *  Created on 19 February 2014
 *      By: Synergiance
 */

// Local Imports
#include "miscbotlib.h"
#include "posrgx.h"

// Global Imports
#include <string>
#include <vector>
#include <regex>
#include <random>
#include <cctype>
#include <climits>

// Boost
//#include <boost/regex.hpp>

// Debug use only
//#include <iostream>

using namespace std;

string toLower(string str)
{// Converts a string to lowercase
    string strReturn;
    for (unsigned int index = 0; index<str.length(); index++)
        if (isalpha(str[index]))
            strReturn += tolower(str[index]);
        else
            strReturn += str[index];
    return strReturn;
}

string toUpper(string str)
{// Converts a string to uppercase
    string strReturn;
    for (unsigned int index = 0; index<str.length(); index++)
        if (isalpha(str[index]))
            strReturn += toupper(str[index]);
        else
            strReturn += str[index];
    return strReturn;
}

void trimWhite(string& text)
{// Trim Whitespace
    while ((text.compare("") != 0) && (isspace(text.at(0))))
        text.erase(text.begin());
    while ((text.compare("") != 0) && (isspace(text.at(text.size() - 1))))
        text.erase(text.end() - 1);
    return;
}

string makeBorder(string title)
{
    string str = "-";
    int borderLength = 40 - title.length() / 2 - 1;
    while (borderLength-- > 0)
        str += '-';
    str += toUpper(title);
    while (str.length() < 80)
        str += '-';
    return str;
}

bool getFirstWord(string message, string& first, string& rest)
{// This method separates a word from the rest of a string
    bool ecaStatus = false;
    int tmp; string str = message;
    trimWhite(str);
    tmp = str.find(" ");
    if (str.compare("") != 0 && tmp != 0)
    {// Make sure string is not blank
        if (tmp == -1)
        {// Only one word
            first = str;
            rest = string();
        }
        else
        {// Multiple words
            first = str.substr(0, tmp++);
            rest = str.substr(tmp, str.size() - tmp);
            trimWhite(rest);
        }
        ecaStatus = true;
    } else {
        first = string();
        rest = string();
    }
    return ecaStatus;
}

bool matchesChars(char chr, string str)
{
    bool retVal = false;
    for (unsigned char c = 0; !retVal && c < str.size() && c <= UCHAR_MAX; c++)
        if (chr == str[c]) retVal = true;
    return retVal;
}

bool parseUser(string fqdn, string& nick, string& user, string& host)
{
    int a = fqdn.find("!");
    int b = fqdn.find("@");
    bool ret = false;
    if (!(a == -1 || b == -1)) {
        nick = fqdn.substr(0,a++);
        user = fqdn.substr(a,b-a);
        host = fqdn.substr(b+1);
        ret = true;
    }
    return ret;
}

void compare(string str1, string str2, unsigned& begin, unsigned& end)
{
    for (begin = 0; begin < str1.size() && begin < str2.size(); begin++)
        if (str1[begin] != str2[begin]) break;
    for (end = 1; end < str1.size() && end < str2.size(); end++)
        if (str1[str1.size() - end] != str2[str2.size() - end]) break;
    end--;
}

// Regex forwarders
bool rgxMatch(string str1, string str2)
{ return posrgx_match(str1, str2); }
bool rgxSearch(string str1, string str2)
{ return posrgx_search(str1, str2); }
string rgxReturn(string str1, string str2)
{ return posrgx_return(str1, str2); }
string rgxReplace(string str1, string str2, string str3)
{ return posrgx_replace(str1, str2, str3); }

/* Old boost/c++11 code (for c++11 remove boost::)
// Regex forwarders
bool rgxMatch(string str1, string str2)
{ return boost::regex_match(str1, boost::regex(str2)); }
bool rgxSearch(string str1, string str2)
{ return boost::regex_search(str1, boost::regex(str2)); }
string rgxReturn(string str1, string str2) {
    boost::smatch m;
    boost::regex_search(str1, m, boost::regex(str2));
    return m.str();
}
string rgxReplace(string str1, string str2, string str3)
{ return boost::regex_replace(str1, boost::regex(str2), str3); }
*/

// Unicode
long fromUnicode(string str)
{// This function will take a unicode character and convert it into an int
    size_t len = unicodeLen(str[0]);
    unsigned char a = 0;
    unsigned char b = 128;
    size_t c;
    long val = 0;
    long tmp = 1;
    if (len != str.size()) return -1;
    for (c = 1; c < len; c++) {
        a = str[len - c];
        a -= 128;
        if (a >= 64) return -1;
        val += a * tmp;
        tmp *= 64;
    }
    a = str[0];
    for (c = 0; c < len; c++) {
        a -= b;
        b /= 2;
    }
    if (a >= b) return -1;
    val += a * tmp;
    return val;
}

string toUnicode(long val)
{// This forms a unicode character from it's value
    unsigned long tmpVal = val;
    if (tmpVal >= 2147483648) return "";
    short c = 0;
    unsigned char a;
    unsigned char b = 128;
    unsigned char d = 2;
    string str = "";
    if (tmpVal < 128) {str = tmpVal; return str;}
    for (c = 0; c < 6 && tmpVal >= 64; c++) {
        a = char(tmpVal % 64);
        tmpVal /= 64;
        a += 128;
        str.insert(0,1,a);
    }
    a = (char)tmpVal;
    for (short e = 5 - c; e > 0; e--) d *= 2;
    if (a >= d) {
        c++;
        a += 128;
        str.insert(0,1,a);
        a = 0;
    }
    for (c++; c > 0; c--) {
        a += b;
        b /= 2;
    }
    str.insert(0,1,a);
    return str;
}

size_t unicodeLen(char c)
{// Gets the length of the unicode sequence
    unsigned char b = 128;
    char a = c;
    size_t d;
    bool confirm = false;
    if (a >= 0) return 1;
    for (d = 0; d < 6 && !confirm; d++) {
        a -= b;
        b /= 2;
        if (a < b) confirm = true;
    }
    if (!confirm || d == 1) return -1;
    return d;
}

vector<long> unidecode(string str)
{// Returns the long values of the chars
    vector<long> ret;
    short charLen;
    long tmp;
    if (str.size() == 0) return ret;
    for (size_t c = 0; c < str.size(); c += charLen) {
        charLen = unicodeLen(str[c]);
        if (charLen == 1) ret.push_back(str[c]);
        if (charLen == -1) continue;
        if ((tmp = fromUnicode(str.substr(c, charLen))) == -1) continue;
        ret.push_back(tmp);
    }
    return ret;
}
