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

using namespace std;

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

    addHi();

    // File Names
    folderName = "lang";
    wordFileName = "words.db";
    phraseFileName = "phrases.txt";
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
{
    return 0;
}

int CEnglish::writePhrases(string fileName)
{
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
