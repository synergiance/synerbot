/*
 * quote.h
 *
 *  Created on 14 February 2014
 *      By: Synergiance
 */

// Local Imports
#include "cmutex.h"
#include "privleges.h"

// Global Imports
#include <string>
#include <vector>
#include <regex>
#include <random>

using namespace std;

#ifndef QUOTES_H_
#define QUOTES_H_

class QuoteHandler
{
public:
    // Constructors
    QuoteHandler(CMutex& buffer, CPrivleges& priv, string chnl);
    ~QuoteHandler();

    void command(string cmd, string args, string talkto, string usr);

    void setVerbosity(bool verbosity);

    void getCapabilities(string& capabilities);
private:
    CMutex *MessageQueue;
    CPrivleges *botPriv;
    mt19937* rnd;

	vector<int> previous_quotes;
	vector<string> quotes;
	// int previous_quotes_limit; // Will be used in the future
    string quoteFile;
	string channel;
    bool addedQuotes;
    bool verboseMode;

    int addQuote(string quote, string sender);
    int remQuote(int pos);
    bool loadQuotes(string file);
    int saveQuotes(string file, bool filtered = false);
    void help(string cmd, string usr, string talkto);
    void say(string target, string message);

    string getQuoter(string& quote);
};

#endif /* QUOTES_H_ */
