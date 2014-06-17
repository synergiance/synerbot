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

#include "english.h"

using namespace std;

/*

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
