/*
 * quote.h
 *
 *  Created on 14 February 2014
 *      By: Tim Gerety
 */

#include <string>
#include <vector>
#include <regex>

using namespace std;

#ifndef QUOTES_H_
#define QUOTES_H_

class QuoteHandler
{
public:
    // Constructors
    QuoteHandler(string chnl);

    void command(string cmd, string args, string talkto, string usr);

    void getCapabilities(string& capabilities);
private:
	vector<int> previous_quotes;
	vector<int> quotes;
	int previous_quotes_limit;
	string channel;
    bool addedQuotes;

    int addQuote(string quote);
    int remQuote(int pos);
    bool loadQuotes(string file);
    int saveQuotes(string file);
    void help(string cmd, string usr, string talkto);
};

#endif /* QUOTES_H_ */
