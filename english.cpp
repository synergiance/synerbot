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

const long flipLetters[26] = {
    0x250, 0x71,  0x254, 0x70,  0x1DD, 0x25F, 0x183, 0x265, 0x1D09,
    0x27E, 0x29E, 0x6C,  0x26F, 0x75,  0x6F,  0x64,  0x62,  0x279,
    0x73,  0x287, 0x6E,  0x28C, 0x28D, 0x78,  0x28E, 0x7A
};

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

CEnglish::CEnglish(bool debug)
{// Constructor
    unsigned rndseed = chrono::system_clock::now().time_since_epoch().count();
    char* rndmem = new char[sizeof(mt19937)];
    rnd = new (rndmem) mt19937(rndseed);

    //addHi();

    // File Names
    folderName = "lang";
    wordFileName = "words.db";
    phraseFileName = "english.lng";

    debugMode = debug;

    //checkDir();

    readPhrases(phraseFileName);
}

void CEnglish::setDebug(bool debug)
{// Sets the debug mode
    debugMode = debug;
    cout<<"Setting English debug module mode "<<debug<<endl;
}

string CEnglish::getHello(string nick, bool only_roman)
{// Returns a hello
    string str;
    int i = searchList("GREETINGS");
    if (i == -1) return "Hi, " + nick;
    uniform_int_distribution<int> dist(0, phrases[i].size() - 1);
    str = phrases[i][dist(*rnd)];
    if (str.find('-') == 0)
    {
        str = str.substr(1, str.size() - 1);
        str += " " + nick;
    }
    return str;
}

string CEnglish::getReply(string nick)
{
    int i = searchList("REPLIES");
    if (i == -1) return "Who, me?";
    uniform_int_distribution<int> dist(0, phrases[i].size() - 1);
    return phrases[i][dist(*rnd)];
}

// # Octatherp
// * Sextile

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
        cout<<"Phrases file found, loading contents\n";
        string strInput;
        int sectionID = -1;
        while (getline(ifile, strInput)) {
            trimWhite(strInput);
            // Check for and remove BOM
            if (strInput.find(utf8bom) == 0) {
                strInput.erase(0,3);
            } else if ((strInput.find(utf16le) == 0)
                    || (strInput.find(utf16be) == 0)) {
                strInput.erase(0,2);
            }
            // Remove carriage return if any
            if (strInput.back() == '\r') strInput.erase(strInput.length() - 1);
            if (strInput.front() == '[' && strInput.back() == ']') {
                string section;
                section = toUpper(strInput.substr(1, strInput.size() - 2));
                if (section.compare("") == 0) {
                    sectionID = -1;
                    continue;
                }
                sectionID = searchList(section);
                if (sectionID == -1) {
                    if (debugMode) cout<<"Adding section: "<<section<<endl;
                    namedList tmp(section);
                    phrases.push_back(tmp);
                    sectionID = phrases.size() - 1;
                }
                if (debugMode) cout<<"Section ID: "<<sectionID<<endl;
            } else if (strInput.compare("") == 0) {
                if (debugMode) cout<<"Empty line.\n";
                continue;
            } else if (sectionID == -1) {
                if (debugMode) cout<<"Line with no section ID\n";
                continue;
            } else if (sectionID >= phrases.size()) {
                if (debugMode) cout<<"If this message appears, I done goofed\n";
                sectionID = -1;
                continue;
            } else {
                phrases[sectionID].push_back(strInput);
            }
        }
        ifile.close();
        if (debugMode) cout<<"Phrases file closed.\n";
    } else {// Uh oh, something went wrong, disable module
        cout<<"Phrases file not found. Please find it.\n";
        namedList hello("GREETINGS");
        hello.push_back("-Hi, ");
        phrases.push_back(hello);
        namedList reply("REPLIES");
        reply.push_back("Who, me?");
        phrases.push_back(reply);
        namedList table("TABLES");
        table.push_back("┻━┻");
        phrases.push_back(table);
        namedList tosser("TOSSERS");
        tosser.push_back("(╯°□°）╯︵ %T");
        phrases.push_back(tosser);
        namedList badcommand("BADCOMMAND");
        badcommand.push_back("Try a real command");
        phrases.push_back(badcommand);
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

string CEnglish::flip(string text)
{// Turns text upside down
    string str;
    if (text.size() == 0) return string();
    vector<long> tmp = unidecode(toLower(text));
    short c;
    for (long unichar: tmp) {
        for (c = 0; c < 26; c++) {
            if (c + 0x61 == unichar) {
                str.insert(0, toUnicode(flipLetters[c]));
                break;
            } else if (flipLetters[c] == unichar) {
                str.insert(0, toUnicode(c + 0x61));
                break;
            }
        }
        if (unichar == 0x20) str.insert(0, " ");
    }
    return str;
}

string CEnglish::toss(string text)
{// Flips a table or an optional string of text
    string str = getRandom("TOSSERS");
    size_t foundChar = 0;
    if (text.size() == 0) {
        for (;;) {
            if ((foundChar = str.find("%T")) == string::npos) break;
            str.erase(foundChar, 2);
            str.insert(foundChar, getRandom("TABLES"));
        }
    } else {
        int numTables = 0;
        int numWords = 1;
        string flipped = flip(text);
        for (;;) {
            if ((foundChar = str.find("%T", foundChar)) == string::npos) break;
            numTables++; foundChar++;
        }
        if (numTables == 1) {
            foundChar = str.find("%T");
            str.erase(foundChar, 2);
            str.insert(foundChar, flipped);
        } else {
            for (int c = 1; c < flipped.size(); c++) {
                if (flipped[c] == 0x20) {
                    if (flipped[c] == flipped[c-1]){
                        flipped.erase(c);
                        c--;
                    } else numWords++;
                }
            }
            vector<string> demWords;
            string tmpStr;
            uniform_int_distribution<int> dist(0, 5);
            size_t word_len = 0, word_pos = 0;
            int randNum, c;
            for (;;) {
                if ((word_len = flipped.find(' ', word_pos)) == string::npos)
                    word_len = flipped.size();
                word_len -= word_pos;
                tmpStr = flipped.substr(word_pos, word_len);
                if (numWords < numTables) {
                    randNum = dist(*rnd);
                    if (randNum > 3 && unidecode(tmpStr).size() >= 8) {
                        string str1, str2;
                        unisplit(tmpStr, str1, str2);
                        demWords.push_back(str1);
                        demWords.push_back(str2);
                        numWords++;
                    } else if (randNum == 3) {
                        demWords.push_back(tmpStr);
                        demWords.push_back(getRandom("TABLES"));
                        numWords++;
                    } else {
                        demWords.push_back(tmpStr);
                    }
                } else {
                    demWords.push_back(tmpStr);
                }
                word_pos += word_len + 1;
                if (word_pos >= flipped.size()) break;
            }
            if (demWords.size() > numTables) {
                short lengths [numTables];
                short tmp = short(demWords.size() / numTables);
                for (c = 0; c < numTables; c++) lengths[c] = tmp;
                tmp = short(demWords.size() % numTables);
                uniform_int_distribution<int> dist(0, numTables - 1);
                for (c = 0; c < tmp; c++) lengths[dist(*rnd)] += 1;
                vector<string> temporaryList;
                for (string word: demWords) temporaryList.push_back(word);
                demWords.clear();
                int d;
                for (c = 0; c < numTables; c++) {
                    demWords.push_back(temporaryList[0]);
                    for (d = 1; d < lengths[c]; d++)
                        demWords.back() += " " + temporaryList[d];
                    for (d = 0; d < lengths[c]; d++)
                        temporaryList.erase(temporaryList.begin());
                }
                numWords = demWords.size();
            }
            for (c = 0; c < numTables; c++) {
                uniform_int_distribution<int> dist(1, numTables - c);
                if ((foundChar = str.find("%T")) == string::npos) break;
                str.erase(foundChar, 2);
                int a = dist(*rnd);
                if (a <= demWords.size()) {
                    str.insert(foundChar, demWords.back());
                    demWords.pop_back();
                }
                else str.insert(foundChar, getRandom("TABLES"));
            }
        }
    }
    return str;
}

void CEnglish::unisplit(string str, string& begin, string& end)
{// Split in the middle unicode style
    vector<long> mischief = unidecode(str);
    begin = string();
    end = string();
    for (int c = 0; c < mischief.size(); c++) {
        if (c < mischief.size() / 2) {
            begin.append(toUnicode(mischief[c]));
        } else {
            end.append(toUnicode(mischief[c]));
        }
    }
}

int CEnglish::searchList(string str)
{// Simple search algorithm
    for (int c = 0; c < phrases.size(); c++)
    {
        if (toUpper(str).compare(phrases[c].name()) == 0) return c;
    }
    return -1;
}

string CEnglish::getRandom(string str)
{// Return a random string from named list
    int i = searchList(str);
    if (i == -1) return string();
    return getRandom(i);
}

string CEnglish::getRandom(int i)
{// Return a random string from specified list
    if (i < 0 || i >= phrases.size()) return string();
    uniform_int_distribution<int> dist(0, phrases[i].size() - 1);
    return phrases[i][dist(*rnd)];
}

// namedList functions
void namedList::push_back(string str)
{ list.push_back(str); }
int namedList::size()
{ return list.size(); }
string &namedList::operator[] (int n)
{ return list[n]; }
string namedList::name()
{ return listName; }
namedList::namedList(string name)
{ listName = name; }
