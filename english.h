/*
 * english.h
 *
 *  Created on 21 April 2014
 *      By: Synergiance
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
    long long sentenceStrength;
    long long followUpStrength;
};

struct word
{
    string word;
    string type;
    bool explitive;
    bool locked;
    vector<wordLink> links;
};

struct namedList
{
    string name();
    namedList(string name);
    vector<string> list;
    void push_back(string str);
    size_t size();
    string &operator[] (size_t n);
private:
    string listName;
};

class CEnglish
{
public:
    // Constructors
    CEnglish(bool debug = true);
    //CEnglish(string file);
    //virtual ~CEnglish();

    void setDebug(bool debug);

    // Access functions
    //string getNoun(bool daddylang, int tense);
    //string getVerb(bool daddylang, int tense);
    //string getAdjective(bool daddylang);
    //string getAdverb(bool daddylang);

    string getHello(string nick, bool only_roman);
    string getReply(string nick);
    bool isHello(string str);

    string toss(string text = "");
    string flip(string text);

    string getRandom(string str);
private:
    mt19937* rnd;
    bool debugMode;
    vector<namedList> phrases;

    // Files
    string folderName;
    string wordFileName;
    string phraseFileName;

    // Dictionary
    vector<word> nouns;
    vector<word> verbs;
    vector<word> adjectives;
    vector<word> adverbs;

    //vector<string> hellos;
    //vector<string> replies;

    //void loadDictionary();
    //void saveDictionary();

    size_t searchList(string str);
    string getRandom(size_t i);

    void unisplit(string str, string& begin, string& end);

    // File IO
    int readWords(string fileName);
    int writeWords(string fileName);
    int readPhrases(string fileName);
    int writePhrases(string fileName);
    int checkDir();

    // Sentence Construction
    //string makeSentence(string related);
    //string nounPhrase(string related);
};

#endif /* CENGLISH_H_ */
