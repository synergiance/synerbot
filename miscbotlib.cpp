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
    for (int index = 0; index<str.length(); index++)
        if (isalpha(str[index]))
            strReturn += tolower(str[index]);
        else
            strReturn += str[index];
    return strReturn;
}

string toUpper(string str)
{// Converts a string to uppercase
    string strReturn;
    for (int index = 0; index<str.length(); index++)
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
