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

using namespace std;

#ifndef CENGLISH_H_
#define CENGLISH_H_

class CEnglish
{
public:
    // Constructors
    CEnglish();
    CEnglish(string file);
    //virtual ~CEnglish();

    // Access functions
    string getNoun(bool explicit, int tense);
    string getVerb(bool explicit, int tense);
    string getAdjective(bool explicit);
    string getAdverb(bool explicit);
private:
    // Dictionary
    vector<string> nouns;
    vector<string> verbs;
    vector<string> adjectives;
    vector<string> adverbs;

    void loadDictionary();
    void saveDictionary();
};

#endif /* CENGLISH_H_ */
