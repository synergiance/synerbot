/*
 * quote.cpp
 *
 *  Created on 14 February 2014
 *      By: Tim Gerety
 */

// Local Imports
#include "quote.h"
#include "cmutex.h"
#include "privleges.h"

// Global Imports
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include <random>
#include <time.h>
#include <sstream>

using namespace std;

QuoteHandler::QuoteHandler(CMutex& buffer, CPrivleges& priv, string chnl)
{
    unsigned rndseed = chrono::system_clock::now().time_since_epoch().count();
    char* rndmem = new char[sizeof(mt19937)];
    rnd = new (rndmem) mt19937(rndseed);

    MessageQueue = &buffer;
    botPriv = &priv;
    channel = chnl;

    quoteFile = "quotes.txt";
    addedQuotes = false;
    loadQuotes(quoteFile);

    verboseMode = false;
}

QuoteHandler::~QuoteHandler()
{
    saveQuotes(quoteFile);
}

void QuoteHandler::setVerbosity(bool verbosity)
{
    verboseMode = verbosity;
}

void QuoteHandler::say(string target, string message)
{
    MessageQueue->push("SAY " + target + " " + message);
}

void QuoteHandler::command(string cmd, string args, string talkto, string usr)
{
    bool admin = botPriv->checkUsr(usr);
    if (cmd.compare("help") == 0) help(args, usr, talkto);
    else if (cmd.compare("add") == 0)
    {// Add a quote
        int tmpq;
        tmpq = addQuote(args);
        if (tmpq == -1)
        {
            if (verboseMode) cout<<"Quote already exists:\n"<<args<<endl;
            say(talkto, "Quote already exists");
        } else if (tmpq == 0) {
            if (verboseMode) cout<<"Adding quote:\n"<<args<<endl;
            say(talkto, "Quote added");
        } else {
            if (verboseMode) cout<<"Quote null\n";
            say(talkto, "Invalid quote: Null");
        }
    }
    else if (cmd.compare("num") == 0)
    {
        if (verboseMode) cout<<"There are "<<quotes.size()<<" quotes loaded\n";
        stringstream ss;
        ss<<"I have "<<quotes.size()<<" quotes loaded";
        say(talkto, ss.str());
    }
    else if (admin && ((cmd.compare("show") == 0)
                    || (cmd.compare("remove") == 0)
                    || (cmd.compare("rem") == 0)))
    {
        int intTmp = atoi(args.c_str());
        if (intTmp > 0)
        {
            intTmp--; unsigned int uIntTmp = intTmp;
            if (uIntTmp < quotes.size())
            {
                if (cmd.compare("show") == 0)
                {
                    if (verboseMode) cout<<"Reciting quote "<<intTmp<<endl;
                    say(talkto, quotes[intTmp]);
                }
                if (cmd.compare("remove") == 0)
                {
                    if (verboseMode) cout<<"Removing quote "<<intTmp<<endl;
                    remQuote(intTmp);
                    say(talkto, "Quote successfully removed");
                }
            }
            else
            {
                stringstream ss;
                if (verboseMode)
                    cout<<"Value entered is greater than number of quotes\n";
                ss<<"There are only "<<quotes.size()<<" quotes in database"
                  <<", please enter a value less than "<<quotes.size();
                say(talkto, ss.str());
            }
        }
        else if (intTmp < 0)
        {
            if (verboseMode)
                cout<<"Value entered was not positive: "<<intTmp<<endl;
            say(talkto, "Please enter a number greater than zero");
        }
        else if (args.compare("0") == 0)
        {
            if (verboseMode) cout<<"Value entered was 0\n";
            say(talkto, "Please enter a number greater than zero");
        }
        else if (args.compare("101702100412530688") == 0)
        {
            if (verboseMode) cout<<"easter egg\n";
            say(talkto, "Let's try a number less than that");
        }
        else
        {
            if (verboseMode) cout<<"No number detected\n";
            say(talkto, "Usage: quote " + cmd + " <number>");
        }
    }
    else if ((cmd.compare("show") == 0)
          || (cmd.compare("remove") == 0)
          || (cmd.compare("rem") == 0))
    {
        if (verboseMode) cout<<"Unauthorized access to "<<cmd<<endl;
        say(talkto, "You are not authorized to use that command");
    }
    else if (cmd.compare("") == 0 || cmd.compare("say") == 0)
    {
        if (quotes.size() > 0) {
            if (verboseMode) cout<<"Selecting random quote: ";
            uniform_int_distribution<int> dist(0, quotes.size() - 1);
            int intTmp = dist(*rnd);
            if (verboseMode) cout<<intTmp<<endl;
            say(talkto, quotes[intTmp]);
        }
        else
            say(talkto, "No quotes");
    }
    else
    {
        if (verboseMode) cout<<"Issuing help on quote\n";
        // Need to expand here
    }
}

int QuoteHandler::addQuote(string quote)
{// Adds a quote the quote file and checls to see if it already exists
    bool alreadyTaken = false;
    trimWhite(quote);
    if (quote.compare("") != 0)
    {// Check to see the quote exists
        if (quotes.size() > 0)
            for (long index = 0;
                        (index<(long)quotes.size() && (alreadyTaken == false));
                        ++index)
                if (quote.compare(quotes.at(index)) == 0)
                    alreadyTaken = true;
        if (alreadyTaken == false)
        {// Add quote
            quotes.push_back(quote);
            addedQuotes = true;
            return 0;
        } else
            return -1;
    } else
        return -2;
}

int QuoteHandler::remQuote(int pos)
{// Removes quote at 0 based index, returns -1 if index is out of bounds
    int tmpRet = 0; unsigned int uPos = pos;
    if ((pos >= 0) && (uPos < quotes.size()))
    {
        quotes.erase(quotes.begin()+pos);
        addedQuotes = true;
    }
    else
        tmpRet = -1;
    return tmpRet;
}

bool QuoteHandler::loadQuotes(string file)
{// Loads from the quotes file, returns true if successful
    bool fState = false;
    ifstream qfile (file.c_str());
    if (qfile)
    {// Determine if file exists
        string strInput;
        cout<<"Loading quotes from "<<file<<" into memory...\n";
        while (getline(qfile, strInput))
        {// Load a line
            trimWhite(strInput);
            // Insert string into vector if it isn't blank
            if (strInput.compare("") != 0)
                quotes.push_back(strInput);
        }
        qfile.close();
        cout<<"Loaded "<<quotes.size()<<" quotes\n";
        fState = true;
    } else {
        cout<<file<<" does not exist, please add quotes\n";
    }
    return fState;
}

int QuoteHandler::saveQuotes(string file)
{// Saves quotes buffer to file, returns 0 if successful
    int fState = 0;
    if (addedQuotes)
    {// Check to see thata the quote buffer isn't empty
        ofstream qfile (file.c_str(), ios::out | ios::trunc);
        if (qfile)
        {// Since the file opened, we can write to it
            cout<<"Writing "<<quotes.size()<<" quotes to file...";
            for (long index = 0; index<(long)quotes.size(); ++index)
                qfile<<quotes.at(index)<<endl;
            qfile.close();
            cout<<" Done\n";
        } else {
            cout<<"File error, no quotes saved\n";
            fState = -1;
        }
    } else {
        cout<<"No buffered quotes to save\n";
        fState = 1;
    }
    return fState;
}

void QuoteHandler::getCapabilities(string& capabilities)
{
    capabilities = "quote";
}

void QuoteHandler::help(string cmd, string usr, string talkto)
{// Take that pesky help section of code and move it here
    if ((cmd.compare("show") == 0)) // Will require admin
    {
        if (verboseMode) cout<<"Help Show command used\n";
        say(talkto, "QUOTE SHOW:");
        say(talkto, "Shows the specified quote");
        say(talkto, "Usage: quote show <number>");
    }
    else if ((cmd.compare("remove") == 0)) // Will require admin
    {
        if (verboseMode) cout<<"Help Remove command used\n";
        say(talkto, "QUOTE REMOVE:");
        say(talkto, "Removes the specified quote");
        say(talkto, "Usage: quote remove <number>");
    }
    else if (cmd.compare("add") == 0)
    {
        if (verboseMode) cout<<"Help Add command used\n";
        say(talkto, "QUOTE ADD:");
        say(talkto, "Adds a new quote to the bot");
        say(talkto, "Usage: quote add <quote>");
    }
    else if (cmd.compare("num") == 0)
    {
        if (verboseMode) cout<<"Help Num command used\n";
        say(talkto, "QUOTE NUM:");
        say(talkto, "Returns the amount of quotes");
        say(talkto, "Usage: quote num");
    }
    else if (cmd.compare("help") == 0)
    {
        if (verboseMode) cout<<"Help Help command used (quote)\n";
        say(talkto, "QUOTE HELP:");
        say(talkto, "Shows help on this command");
        say(talkto, "Usage: quote help [<topic>]");
    }
    else
    {
        if (verboseMode) cout<<"Help command used (Quote)\n";
        say(talkto, "QUOTE:");
        say(talkto, "Recites a quote at random");
        say(talkto, "Usage: quote");
        say(talkto, "Subcommands: show, remove, add, num, help");
    }
}
