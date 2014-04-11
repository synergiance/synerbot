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
    QuoteHandler();

    int addQuote(string quote);
    int remQuote(int pos);
    bool loadQuotes(string file);
    int saveQuotes(string file);
    void getCapabilities(string& capabilities);
private:
};

#endif /* QUOTES_H_ */
