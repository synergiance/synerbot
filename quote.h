/*
 * quote.h
 *
 *  Created on 14 February 2014
 *      By: Tim Gerety
 */

#include <string>
#include <vector>

using namespace std;

#ifndef QUOTES_H_
#define QUOTES_H_

class QuoteHandler
{
public:
    // Constructors
    QuoteHandler(string chnl);

    void command(string command, string user, string talkto);

    void getCapabilities(string& capabilities);
private:
	vector<int> previous_quotes;
	vector<int> quotes;
	int previous_quotes_limit;
	string channel;

    int addQuote(string quote);
    int remQuote(int pos);
    bool loadQuotes(string file);
    int saveQuotes(string file);
};

#endif /* QUOTES_H_ */
