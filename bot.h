/*
 * bot.h
 *
 *  Created on 30 May 2013
 *      By: Synergiance
 */

#include <string>
#include <vector>
#include <fstream>

#include "config.h"
#include "privleges.h"
#include "net.h"
#include "cmutex.h"

using namespace std;

#ifndef IRCBOT_H_
#define IRCBOT_H_

class IrcBot
{
public:
    IrcBot(string cfg, int bDebug, bool bVerbose);
    virtual ~IrcBot();

    bool setup;

    void start();
    void stop();

private:
    int s; //the socket descriptor

    // Config variables
    int debugMode;
    string nick;
    string usr;
    string realName;
    string server;
    string cfgFile;
    string quoteFile;
    string serverName;
    string channelName;
    string port;

    CConfig* botConfig;
    CPrivleges* botPriv;
    CNetSocket* botSock;
    CMutex* MessageQueue;

    // Important static values
    static long atoimax;

    // Variables
    vector<string> quotes;
    vector<string> bufquotes;

    bool addedQuotes, verboseMode;
    volatile bool stopping;

    // Functions
    char * timeNow();

    bool sendData(char *msg);
    bool sendData(string msg);

    void sendPong(string data);

    bool globalHandle(string cmd);
    void msgHandel(string buf);

    int msgParse(string buf, string& sender, string& message, string& cmd);

    void AI(string sender, string cmd, string msg);

    int commandHandle(string cmd, string args, string talkto, bool admin);
    bool extractCommandArgs(string message, string& command, string& args);

    void say(string target, string message);
    void action(string target, string message);

    int addQuote(string quote);
    int remQuote(int pos);
    bool loadQuotes(string file);
    int saveQuotes(string file);

    // User Functions
    void quote(string cmd, string args, string talkto, bool admin);
    void editPrivs(string cmd, string args, string talkto);

    // Utils
    void whois(string target);
};

#endif /* IRCBOT_H_ */
