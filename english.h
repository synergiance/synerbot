/*
 * english.h
 *
 *  Created on 21 April 2014
 *      By: Tim Gerety
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <random>

using namespace std;

#ifndef CENGLISH_H_
#define CENGLISH_H_

struct wordLink
{
    string word;
    int sentenceStrength;
    int followUpStrength;
};

struct word
{
    string word;
    string types;
    string nounPlural;
    string verbPast;
    string verbPresent;
    string verbcupation;
    bool explitive;
    bool locked;
    vector<wordLink> links;
};

class CEnglish
{
public:
    // Constructors
    CEnglish();
    //CEnglish(string file);
    //virtual ~CEnglish();

    // Access functions
    //string getNoun(bool daddylang, int tense);
    //string getVerb(bool daddylang, int tense);
    //string getAdjective(bool daddylang);
    //string getAdverb(bool daddylang);

    string getHello(string nick, bool only_roman);
private:
    mt19937* rnd;

    // Files
    string folderName;
    string wordFileName;
    string phraseFileName;

    // Dictionary
    vector<string> nouns;
    vector<string> verbs;
    vector<string> adjectives;
    vector<string> adverbs;

    vector<string> hellos;

    //void loadDictionary();
    //void saveDictionary();

    void addHi();

    // File IO
    int readWords(string fileName);
    int writeWords(string fileName);
    int readPhrases(string fileName);
    int writePhrases(string fileName);
    int checkDir();
};

#endif /* CENGLISH_H_ */
