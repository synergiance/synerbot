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

    // Dictionary
    vector<string> nouns;
    vector<string> verbs;
    vector<string> adjectives;
    vector<string> adverbs;

    vector<string> hellos;

    //void loadDictionary();
    //void saveDictionary();

    void addHi();
};

struct word
{
    string word;
    string types;
    bool explicit;
};

#endif /* CENGLISH_H_ */
