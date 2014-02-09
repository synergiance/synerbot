/*
 * bot.h
 *
 *  Created on 30 May 2013
 *      By: Synergiance
 */

#include <string>
#include <vector>
#include <fstream>

using namespace std;

#ifndef IRCBOT_H_
#define IRCBOT_H_

class IrcBot
{
public:
    IrcBot(string cfg);
    virtual ~IrcBot();

    bool setup;

    void start();

private:
    int s; //the socket descriptor

    // Config variables
    string nick;
    string usr;
    string realName;
    string server;
    string cfgFile;
    string quoteFile;
    string serverName;
    string channelName;
    string port;

    // Important static values
    static long atoimax;

    // Variables
    vector<string> quotes;
    vector<string> bufquotes;

    bool addedQuotes;

    // Functions
    char * timeNow();

    bool sendData(char *msg);
    bool sendData(string msg);

    void sendPong(string data);

    void msgHandel(string buf);

    int msgParse(string buf, string& sender, string& message);

    void AI(string sender, string msg);

    int commandHandle(string cmd, string args, string talkto, bool admin);
    bool extractCommandArgs(string message, string& command, string& args);

    void say(string target, string message);
    void action(string target, string message);


    int addQuote(string quote);
    bool loadQuotes(string file);
    int saveQuotes(string file);

    void trimWhite(string& text);

    // User Functions
    void adminQuote(string cmd, string args, string talkto);
};

#endif /* IRCBOT_H_ */
