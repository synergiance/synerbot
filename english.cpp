/*
 * english.cpp
 *
 *  Created on 21 April 2014
 *      By: Tim Gerety
 */

// Global Imports
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <time.h>
#include <iostream>
#include <fstream>

// Linux Imports
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// Local Imports
#include "english.h"
#include "miscbotlib.h"

using namespace std;

const char utf8bom[4] = {(char)239, (char)187, (char)191, 0};
const char utf16be[3] = {(char)254, (char)255, 0};
const char utf16le[3] = {(char)255, (char)254, 0};

/*

It feels strange reading this but:
http://en.wikipedia.org/wiki/English_grammar

Ideas:

VERBS:
EVERBS:
(+er, +ing, +s, +ed)

NOUNS:
ENOUNS:
(+s)

ADJECTIVES:
EADJECTIVES:
(+er, +est)

ADVERBS:
EADVERBS:

WORD:
Explicit
Types array
links array with ratings

sentence ideas:
[adjective] noun verb [adjective] noun [adverb]
[adjective] noun verb [adverb]

noun: Determiner + Pre-modifiers + NOUN + Postmodifiers/Complement

*/

CEnglish::CEnglish()
{// Constructor
    unsigned rndseed = chrono::system_clock::now().time_since_epoch().count();
    char* rndmem = new char[sizeof(mt19937)];
    rnd = new (rndmem) mt19937(rndseed);

    //addHi();

    // File Names
    folderName = "lang";
    wordFileName = "words.db";
    phraseFileName = "english.lng";

    //checkDir();

    readPhrases(phraseFileName);
}

void CEnglish::setDebug(bool debug)
{// Sets the debug mode
    debugMode = debug;
}

string CEnglish::getHello(string nick, bool only_roman)
{// Returns a hello
    string str;
    uniform_int_distribution<int> dist(0, hellos.size() - 1);
    str = hellos[dist(*rnd)];
    if (str.find('-') == 0)
    {
        str = str.substr(1, str.size() - 1);
        str += " " + nick;
    }
    return str;
}

string CEnglish::getReply(string nick)
{
    uniform_int_distribution<int> dist(0, replies.size() - 1);
    return replies[dist(*rnd)];
}

// # Octatherp
// * Sextile
/* This code should be unneeded
void CEnglish::addHi()
{// TODO: Make dynamic
    hellos.push_back("-Hi");
    hellos.push_back("Hello");
    hellos.push_back("Konnichiwa");
    hellos.push_back("Konnichiwa");
    hellos.push_back("Moshi moshi");
    hellos.push_back("こんにちは。");
    hellos.push_back("-Hello,");
    hellos.push_back("Hi");
    hellos.push_back("-Hi,");
    hellos.push_back("Hello");
    hellos.push_back("-Hi");
    hellos.push_back("-Hello");
    hellos.push_back("Hi");
    hellos.push_back("-Hi,");
    hellos.push_back("Hello");
    hellos.push_back("-Hi");
    hellos.push_back("-Hello,");
    hellos.push_back("Greetings");
    hellos.push_back("-Greetings,");
    hellos.push_back("-Greetings,");
    hellos.push_back("Ahoy");
    hellos.push_back("G'day");
    hellos.push_back("Hello there");
    hellos.push_back("Hey");
    hellos.push_back("Hi there");
    hellos.push_back("Howdy");
    hellos.push_back("Salutations");
    hellos.push_back("Yo");
    hellos.push_back("Hiya");
    hellos.push_back("-Ahoy,");
    hellos.push_back("G'day mate");
    hellos.push_back("-Hello there");
    hellos.push_back("-Hey");
    hellos.push_back("-Hi there");
    hellos.push_back("-Hiya");
    hellos.push_back("-G'day,");
}
*/

/*

Word File Structure:

conjunctions {
    and
    or
    but
    but not
}

wordname {
    noun
    connections {
        word,21,32
        other,43,9
    }
}

Synergiance {
    pronoun
    connections {
        word,0,0
        other,2,1
    }
}

fuck {
    verb
    explitive
    connections {
        word,3,2
        other,5,3
    }
}

*/

int CEnglish::readWords(string fileName)
{
    ifstream ifile (fileName.c_str());
    return 0;
}

int CEnglish::writeWords(string fileName)
{
    checkDir();
    return 0;
}

int CEnglish::readPhrases(string fileName)
{// Reads the phrase file
    ifstream ifile (fileName.c_str());
    if (ifile) {// All good, the file exists lets proceed
        string section, strInput;
        while (getline(ifile, strInput)) {
            trimWhite(strInput);
            // Check for and remove BOM here - TODO
            if (strInput.find(utf8bom) == 0) {
                strInput.erase(0,3);
            } else if ((strInput.find(utf16le) == 0)
                    || (strInput.find(utf16be) == 0)) {
                strInput.erase(0,2);
            }
            if (strInput.back() == '\r') strInput.erase(strInput.length() - 1);
            if (strInput.front() == '[' && strInput.back() == ']') {
                section = strInput.substr(1, strInput.size() - 2);
            } else if (strInput.compare("") == 0) {
                continue;
            } else if (section.compare("") == 0) {
                continue;
            } else if (toUpper(section).compare("GREETINGS") == 0) {
                hellos.push_back(strInput);
            } else if (toUpper(section).compare("REPLIES") == 0) {
                replies.push_back(strInput);
            }
        }
        ifile.close();
    } else {// Uh oh, something went wrong, disable module
        hellos.push_back("-Hi");
        replies.push_back("Who, me?");
        return -1;
    }
    return 0;
}

int CEnglish::writePhrases(string fileName)
{// Writes changes to the phrase file
    checkDir();
    return 0;
}

int CEnglish::checkDir()
{// Separate function in case I need to mess with OS specifics
    struct stat sb = {0};
    if (stat(folderName.c_str(), &sb) == -1)
        mkdir(folderName.c_str(), 0644);
    return 0;
}
