/*
 * bot.cpp
 *
 *  Created on: 31 May 2013
 *      Author: Synergiance
 *
 */

// Local Includes
#include "bot.h"
#include "config.h"
#include "privleges.h"
#include "miscbotlib.h"
#include "net.h"
#include "cmutex.h"
#include "english.h"
#include "quote.h"

// Global Includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <random> // Convert all rand() to random
#include <regex>

// RFCs to check out: 2810, 2811, 2812, 2813
// http://www.irchelp.org/irchelp/rfc/

using namespace std;

#define MAXDATASIZE 100

long IrcBot::atoimax = 101702100412530687; // Wrong

IrcBot::IrcBot(string cfg, int bDebug, bool bVerbose)
{// Constructor to get everything all set
    debugMode = bDebug;
    verboseMode = bVerbose;

    unsigned rndseed = chrono::system_clock::now().time_since_epoch().count();

    stopping = false;

    // Allocate memory
    char* bcfgint = new char[sizeof(CConfig)];
    char* bprmint = new char[sizeof(CPrivleges)];
    char* netmem = new char[sizeof(CNetSocket)];
    char* msgmem = new char[sizeof(CMutex)];
    char* engmem = new char[sizeof(CEnglish)];
    char* rndmem = new char[sizeof(mt19937)];
    char* qtsmem = new char[sizeof(QuoteHandler)];

    // Set modules
    botConfig = new (bcfgint) CConfig(cfg);
    botPriv = new (bprmint) CPrivleges();
    EngLang = new (engmem) CEnglish();

    rnd = new (rndmem) mt19937(rndseed);

    // Load configuration
    cfgFile = cfg;
    nick = botConfig->getNick();
    usr = botConfig->getUsr();
    realName = botConfig->getRealName();
    server = botConfig->getServer();
    serverName = botConfig->getServerName();
    channelName = botConfig->getChannelName();
    port = botConfig->getPort();

    // Will hopefully be deprecated
    /* Disabling
    quoteFile = "quotes.txt";
    addedQuotes = false;
    loadQuotes(quoteFile);
    */

    rgxHello = "\\b(hi|hello|greetings|hey|ahoy|g'day|howdy|yo|hiya),? "
             + toLower(nick) + "\\b";

    // Set other modules
    if (debugMode == 8 || debugMode == 13)
        MessageQueue = new (msgmem) CMutex(true);
    else
        MessageQueue = new (msgmem) CMutex();
    botSock = new (netmem) CNetSocket(server, port, *MessageQueue, bDebug);
    CQuotes = new (qtsmem) QuoteHandler(*MessageQueue, *botPriv, channelName);
    CQuotes->setVerbosity(bVerbose);
}

IrcBot::~IrcBot()
{// Tell thread to stop when the bot gets destroyed
    delete botSock;
    delete CQuotes;
    delete MessageQueue;
    delete botConfig;
    delete botPriv;
    delete EngLang;
    delete rnd;
    //saveQuotes(quoteFile);
}

void IrcBot::stop()
{// Interrupt method to signal the bot to stop
    stopping = true;
}

void IrcBot::start()
{
    bool keepRunning = true, moreBuffer = false, checkAgain = false;
    int delay = 1, minDelay = 1, maxDelay = 100;

    // Initialize a random seed
    srand(time(NULL)); // Deprecated

    botSock->botConnect(nick, usr, realName);

    string str, cmd, msg;

    while (keepRunning)
    {// Main loop
        str = ""; cmd = ""; msg = "";

        if (moreBuffer)
            moreBuffer = MessageQueue->pull(str, -1);
        else if (checkAgain)
        {
            moreBuffer = MessageQueue->pull(str, -1);
            checkAgain = false;
        }
        else
            moreBuffer = MessageQueue->pull(str, delay++ * 100);

        if (delay > maxDelay) delay = maxDelay;

        if (stopping) botSock->botDisconnect();
        if (!getFirstWord(str, cmd, msg)) continue;
        if (msg.compare("") == 0) continue;

        checkAgain = true;

        delay /= 4;
        if (delay < minDelay) delay = minDelay;

        if (toUpper(cmd).compare("RAW") == 0) {
            msgHandel(msg);
        }
        else if (toUpper(cmd).compare("GLOBAL") == 0) {
            if (!globalHandle(msg)) keepRunning = false;
        }
        else {
            otherHandle(cmd, msg);
        }
    }
    //saveQuotes(quoteFile);
}


bool IrcBot::sendData(string msg)
{// Send data to network handler
    botSock->toThread("net send " + msg);
    return true;
}


bool IrcBot::globalHandle(string cmd)
{// Global stuff
    string command, message;
    if (debugMode == 30) cout<<"GLOBAL HANDLER COMMAND: "<<cmd<<endl;
    if (!getFirstWord(cmd, command, message)) return true;
    if (toUpper(command).compare("DISCONNECT") == 0)
    { botSock->botDisconnect(); return false; }
    if (toUpper(command).compare("DISCONNECTED") == 0) return false;
    if (toUpper(command).compare("CONNECTED") == 0) cout<<"Connected!\n";
    if (toUpper(command).compare("MOTD") == 0)
    {
        cout<<"Joining "<<channelName<<"\n";
        sendData("JOIN " + channelName + "\r\n");
    }
    if (toUpper(command).compare("COUT") == 0) cout<<message<<endl; // Deprecated
    return true;
}

void IrcBot::otherHandle(string command, string message)
{
    if (debugMode == 30) cout<<"OTHER HANDLER: "<<command<<" "<<message<<endl;
    if (toUpper(command).compare("COUT") == 0) cout<<message<<endl;
    if (toUpper(command).compare("SAY") == 0)
    {
        string channel, channel_message;
        if (getFirstWord(message, channel, channel_message)
                && channel_message.compare("") != 0)
            say(channel, channel_message);
    }
    if (toUpper(command).compare("ACTION") == 0)
    {
        string channel, channel_message;
        if (getFirstWord(message, channel, channel_message)
                && channel_message.compare("") != 0)
            action(channel, channel_message);
    }
}

int IrcBot::msgParse(string buf, string& sender, string& message, string& cmd)
{// Grabs IRC sender, message code, and message
    int tmp, intCode;
    string str = buf;
    
    // Find pings
    if (str.substr(0,4).find("PING") == 0)
    {
        sender = "PING";
        if (str.size() > 7)
            message = str.substr(6, str.size() - 7);
        else
            message = "";
        return -1;
        //break;
    }
    
    // Trim out the fat
    tmp = str.find("\r");
    if (tmp != -1)
        str = str.substr(0,tmp);
    
    // Parse out sender
    tmp = str.find(" ") + 1;
    sender = str.substr(1, tmp - 2);
    str = str.substr(tmp, str.size() - tmp);
    
    // Parse message code
    intCode = atoi(str.substr(0,3).c_str());
    if (intCode == 0)
    { // Text IRC message - Communication
        // Parse out command
        tmp = str.find(" ") + 1;
        cmd = str.substr(0, tmp - 1);
        message = str.substr(tmp, str.size() - tmp);
    }
    else
    { // Numerical IRC message - server messages?
        // Since all IRC codes are 3 digits, this is really easy
        str = str.substr(4, str.size() - 4);

        // Remove receiver
        tmp = str.find(" ") + 1;
        message = str.substr(tmp, str.size() - tmp);
        cmd = "";
    }
    
    return intCode;
}

void IrcBot::msgHandel(string buf)
{// Do stuff with messages
    int code;
    string message;
    string sender;
    string cmd;

    if (debugMode == 6)
        cout<<buf;
    
    // Parse message
    code = msgParse(buf, sender, message, cmd);
    
    if (debugMode == 5)
        if (code > 0)
            cout<<"<"<<sender<<"> ("<<code<<") "<<message<<endl;

    if (code == 0) {
        if (debugMode == 21 || debugMode == 5)
            cout<<"<"<<sender<<"> ("<<cmd<<") "<<message<<endl;
        AI(sender, cmd, message);
    }
    return;
}

void IrcBot::AI(string sender, string cmd, string msg)
{// Deal with humans
    string channel;
    string message;
    string name;
    string command;
    string args;
    
    // Get sender's name
    name = sender.substr(0, sender.find("!"));

    if (toUpper(cmd) == "PRIVMSG")
    {// Only actual messages should make it to this point
        channel = msg.substr(0, msg.find(" "));
        message = msg.substr(channel.size() + 2,
            msg.size() - (channel.size() + 2));
        if (channel.substr(0,1) == "#")
        {// Message is a channel
            // Only output if debug mode is on
            if (verboseMode)
                cout<<name<<" said on "<<channel<<": "<<message<<endl;

            if (regex_match(toLower(message.substr(0, nick.size() + 1)),
                regex(toLower(nick) + "(:|,)")))
            {// Got pinged, determine command
                if (message.size() > nick.size() + 2)
                {// Now we know there's text to parse after the ping
                    message = message.substr(nick.size() + 2,
                        message.size() - (nick.size() + 2));
                    if (extractCommandArgs(message, command, args))
                    {
                        // Print command in console
                        if (verboseMode)
                            cout<<name<<" issued command \""<<command
                                <<"\" on channel "<<channel<<" with"
                                <<((args.compare("") != 0)
                                ? (" parameters: " + args)
                                : " no parameters")<<endl;

                        // Commands go here now, so GIT
                        commandHandle(command, args, channelName, sender);
                    }
                } else {// Someone tried to say hi as if I were a bot
                    say(channel, "I prefer a hello");
                }
            } else if (regex_search(toLower(message), regex(rgxHello)))
                say(channel, EngLang->getHello(name, false));
        } else
        {// Message is a user
            extractCommandArgs(message, command, args);

            // Only output if debug mode is on
            if (verboseMode)
                cout<<name<<" issued command \""<<command<<"\" in a PM with "
                    <<((args.compare("") != 0) ? ("parameters: " + args)
                    : "no parameters")<<endl;

            // Commands go here now, now git
            commandHandle(command, args, name, sender);
        }
    }
    return;
}

void IrcBot::say(string target, string message)
{
    if (verboseMode) cout<<"Saying ("<<target<<"): " + message<<endl;
    sendData("PRIVMSG " + target + " :" + message);
    return;
}

void IrcBot::action(string target, string message)
{
    string a = "\x01";
    if (verboseMode) cout<<"Action ("<<target<<"): " + message<<endl;
    sendData("PRIVMSG " + target + " :" + a + "ACTION " + message + a);
    return;
}

bool IrcBot::loadQuotes(string file)
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

int IrcBot::saveQuotes(string file)
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

int IrcBot::addQuote(string quote)
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

int IrcBot::remQuote(int pos)
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

int IrcBot::commandHandle(string cmd, string args, string talkto, string usr)
{// This method handles all the commands sent to the bot
    int intReturn = 0;

    // Check permissions
    bool admin = botPriv->checkUsr(usr);

    bool cmdMatch = false;
    
    // Normal commands

    // Say a random quote
    if (cmd.compare("quote") == 0) {
        // Forward everything to method
        string subcmd; string subargs;
        if (!(extractCommandArgs(args, subcmd, subargs))) {
            subcmd = "";
            subcmd = "";
        }
        //quote(subcmd, subargs, talkto, admin);
        CQuotes->command(subcmd, subargs, talkto, usr);
        cmdMatch = true;
    } else if (toLower(cmd).compare("augh") == 0) {
        say(talkto, "AUGH"); cmdMatch = true;
    } else if (toLower(cmd).compare("roulette") == 0) {
        say(talkto, "lol, no"); cmdMatch = true;
    }
    
    // Admin commands
    else if (admin) {
        if (cmd.compare("say") == 0 && args.compare("") != 0) {
            say(channelName, args); cmdMatch = true;
        } else if (cmd.compare("action") == 0 && args.compare("") != 0) {
            action(channelName, args);cmdMatch = true;
        } else if (cmd.compare("admin") == 0 && args.compare("") != 0) {
            // Forward everything to method
            string subcmd; string subargs;
            if (!(extractCommandArgs(args, subcmd, subargs)))
            { subcmd = ""; subcmd = ""; }
            editPrivs(subcmd, subargs, talkto);
            cmdMatch = true;
        }
    }

    // No command matched
    if (!cmdMatch)
    {
        say(talkto, "Try a real command");
    }
    return intReturn;
}

void IrcBot::quote(string cmd, string args, string talkto, bool admin)
{
    // cout<<"cmd: "<<cmd<<endl<<"args: "<<args<<endl;
    if (cmd.compare("help") == 0)
    {
        if ((args.compare("show") == 0) && admin)
        {
            if (verboseMode) cout<<"Help Show command used\n";
            say(talkto, "QUOTE SHOW:");
            say(talkto, "Shows the specified quote");
            say(talkto, "Usage: quote show <number>");
        }
        else if ((args.compare("remove") == 0) && admin)
        {
            if (verboseMode) cout<<"Help Remove command used\n";
            say(talkto, "QUOTE REMOVE:");
            say(talkto, "Removes the specified quote");
            say(talkto, "Usage: quote remove <number>");
        }
        else if (args.compare("add") == 0)
        {
            if (verboseMode) cout<<"Help Add command used\n";
            say(talkto, "QUOTE ADD:");
            say(talkto, "Adds a new quote to the bot");
            say(talkto, "Usage: quote add <quote>");
        }
        else if (args.compare("num") == 0)
        {
            if (verboseMode) cout<<"Help Num command used\n";
            say(talkto, "QUOTE NUM:");
            say(talkto, "Returns the amount of quotes");
            say(talkto, "Usage: quote num");
        }
        else if (args.compare("help") == 0)
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
            if (verboseMode) cout<<"Selecting random quote\n";
            uniform_int_distribution<int> dist(0, quotes.size() - 1);
            say(talkto, quotes[dist(*rnd)]);
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

void IrcBot::editPrivs(string cmd, string args, string talkto)
{// Privliges
    if (cmd.compare("add") == 0)
    {// Add a quote
        int tmpq;
        tmpq = botPriv->addUsr(args);
        if (tmpq == -1)
        {
            if (verboseMode) cout<<"User privlege already exists: "<<args<<endl;
            say(talkto, "User privlege already exists");
        } else if (tmpq == 0) {
            if (verboseMode) cout<<"Adding user: "<<args<<endl;
            say(talkto, "User privleges added");
        } else {
            if (verboseMode) cout<<"Null string: User privlege\n";
            say(talkto, "Invalid user: Null");
        }
    }
    else if (cmd.compare("remove") == 0)
    {// Add a quote
        int tmpq;
        tmpq = botPriv->remUsr(args);
        if (tmpq == -1)
        {
            if (verboseMode) cout<<"Off by 1 error: User privleges";
            say(talkto, "Contact bot author");
        } else if (tmpq == 0) {
            if (verboseMode) cout<<"Removing user privlege: "<<args<<endl;
            say(talkto, "User privleges removed");
        } else if (tmpq == -2) {
            if (verboseMode) cout<<"User privlege does not exist: "<<args<<endl;
            say(talkto, "User does not have privleges");
        } else if (tmpq == -3) {
            if (verboseMode) cout<<"Null string: User privlege\n";
            say(talkto, "Invalid user: Null");
        } else {
            if (verboseMode) cout<<"Error: Impossible return value\n";
            say(talkto, "Impossible return value, contact bot author");
        }
    }
}

bool IrcBot::extractCommandArgs(string message, string& command, string& args)
{// This method separates a command from it's arguments
    bool ecaStatus = false;
    int tmp; string str = message;
    trimWhite(str);
    tmp = str.find(" ");
    if (str.compare("") != 0 && tmp != 0)
    {// Bot will not accept too much whitespace
        if (tmp == -1)
        {
            command = str;
            args = "";
        }
        else
        {
            command = str.substr(0, tmp++);
            args = str.substr(tmp, str.size() - tmp);
            trimWhite(args);
        }
        ecaStatus = true;
    }
    return ecaStatus;
}

void IrcBot::whois(string target)
{
    sendData("WHOIS " + target);
}
