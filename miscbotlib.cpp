/*
 * miscbotlib.cpp
 *
 *  Created on 19 February 2014
 *      By: Tim Gerety
 */

// Local Imports
#include "miscbotlib.h"

// Global Imports
#include <string>
#include <vector>
#include <cctype>

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
    while ((text.compare("") != 0) && (text.at(0) == ' '))
        text.erase(text.begin());
    while ((text.compare("") != 0) && (text.at(text.size() - 1) == ' '))
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
            rest = "";
        }
        else
        {// Multiple words
            first = str.substr(0, tmp++);
            rest = str.substr(tmp, str.size() - tmp);
            trimWhite(rest);
        }
        ecaStatus = true;
    }
    return ecaStatus;
}
