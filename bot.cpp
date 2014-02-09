/*
 * bot.cpp
 *
 *  Created on: 31 May 2013
 *      Author: Synergiance
 *
 */

#include "bot.h"
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

using namespace std;

#define MAXDATASIZE 100

long IrcBot::atoimax = 101702100412530687;

IrcBot::IrcBot(string cfg)
{
    // Set defaults
    cfgFile = cfg;
	nick = "synerbot";
	usr = "synbot";
	realName = "The bot of Synergiance";
	server = "127.0.0.1";
	quoteFile = "quotes.txt";
	serverName = "server";
    channelName = "#bots";
    port = "6667";
    
    addedQuotes = false;
    
    // some booleans for ini setting tracking, could save from trickery
    bool bnick; bool busr; bool brealName; bool bserver; bool bport;
    bool bquoteFile; bool bserverName; bool bchannelName;
    
    // Look for the file
    ifstream ifile (cfg.c_str());
    if (ifile)
    {// File exists, read configuration from it
        string strBuffer;
        cout<<"Found "<<cfgFile<<", loading configuration...\n";
        int tmp; string tmpstr; string tmpstr2;
        while (getline(ifile, strBuffer))
        {// Read a line
            tmp = strBuffer.find("=");
            if (tmp > -1 && (tmp + 1) < strBuffer.length())
            {// Determine validity
                tmpstr = strBuffer.substr(0,tmp); tmp++;
                tmpstr2 = strBuffer.substr(tmp, strBuffer.length() - tmp);
                if (!bnick && tmpstr.compare("Nick") == 0)
                {
                    nick = tmpstr2;
                    bnick = true;
                } else if (!busr && tmpstr.compare("Username") == 0)
                {
                    usr = tmpstr2;
                    busr = true;
                } else if (!brealName && tmpstr.compare("Description") == 0)
                {
                    realName = tmpstr2;
                    brealName = true;
                } else if (!bserver && tmpstr.compare("Server") == 0)
                {
                    server = tmpstr2;
                    bserver = true;
                } else if (!bserverName && tmpstr.compare("ServerName") == 0)
                {
                    serverName = tmpstr2;
                    bserverName = true;
                } else if (!bport && tmpstr.compare("Port") == 0)
                {
                    port = tmpstr2;
                    bport = true;
                } else if (!bchannelName && tmpstr.compare("Channel") == 0)
                {
                    channelName = tmpstr2;
                    bchannelName = true;
                } else if (!bquoteFile && tmpstr.compare("QuoteFile") == 0)
                {
                    quoteFile = tmpstr2;
                    bquoteFile = true;
                } else {
                    cout<<"Unknown config setting: "<<tmpstr<<endl;
                }
            }
        }
        cout<<"Loading complete!\n";
        // Always make sure to close your files
        ifile.close();

        // Let's rewrite everything to disk
        ofstream ofile (cfg.c_str());
        if (ofile)
        {// Success
            cout<<"Writing configuration...";
            if (bnick)
                ofile<<"Nick="<<nick<<endl;
            if (busr)
                ofile<<"Username="<<usr<<endl;
            if (brealName)
                ofile<<"Description="<<realName<<endl;
            if (bserver)
                ofile<<"Server="<<server<<endl;
            if (bserverName)
                ofile<<"ServerName="<<serverName<<endl;
            if (bport)
                ofile<<"Port="<<port<<endl;
            if (bchannelName)
                ofile<<"Channel="<<channelName<<endl;
            if (bquoteFile)
                ofile<<"QuoteFile="<<quoteFile<<endl;
            ofile.close(); cout<<" Done\n";
        } else {// Errors
            cout<<", errors writing file, continuing with defaults\n";
        }
    } else {// File does not exist, create it and write defaults
        cout<<"Could not find "<<cfgFile;
        ofstream ofile (cfg.c_str());
        if (ofile)
        {// Success
            cout<<", writing defaults...";
            ofile<<"Nick="<<nick<<endl
                 <<"Username="<<usr<<endl
                 <<"Description="<<realName<<endl
                 <<"Server="<<server<<endl
                 <<"Port="<<port<<endl
                 <<"Channel="<<channelName;
            ofile.close(); cout<<" Done\n";
        } else {// Errors
            cout<<", errors writing file, continuing with defaults\n";
        }
    }
    loadQuotes(quoteFile);
}

IrcBot::~IrcBot()
{
    sendData((char*)"QUIT :Watch out for them, they're coming for you!\r\n");
    close (s);
    saveQuotes(quoteFile);
}

void IrcBot::start()
{
    struct addrinfo hints, *servinfo;

    //Setup run with no errors
    setup = true;

    //Ensure that servinfo is clear
    memset(&hints, 0, sizeof hints); // make sure the struct is empty

    //setup hints
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    //Setup the structs if error print why
    int res;
    if ((res = getaddrinfo(server.c_str(),port.c_str(),&hints,&servinfo)) != 0)
    {
        setup = false;
        fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(res));
    }


    //setup the socket
    if ((s = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) == -1)
    {
        perror("client: socket");
    }

    cout<<"Connecting to "<<server<<" on port "<<port<<" with username "<<nick<<endl;

    //Connect
    if (connect(s,servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        close (s);
        perror("Client Connect");
    }


    //We dont need this anymore
    freeaddrinfo(servinfo);


    //Recv some data
    int numbytes;
    char buf[MAXDATASIZE];

    // Some variables
    int count = 0;
    string str;
    int auth = 0;

    // Initialize a random seed
    srand(time(NULL));

    // Send username info
    sendData("NICK " + nick + "\r\n");
    sendData("USER " + usr + " 8 * :" + realName + "\r\n");

    while (1)
    {
        //declars
        //count++;
        int msgCode; int found;
        string message;
        string sender;
        string rawMessage;


        // Receive data
        numbytes = recv(s,buf,MAXDATASIZE-1,0);
        buf[numbytes]='\0';
        str += buf;

        // Determine if there is a new message
        found = str.find('\n');
        while (found != -1)
        {
            found++; // Fix off by 1 error
            
            // Extract single message and pass to handler
            msgHandel(str.substr(0, found));
            str = str.substr(found, str.size() - found);
            
            // Test again
            found = str.find('\n');
        }

        //break if connection closed
        if (numbytes==0)
        {
            cout << "----------------------CONNECTION CLOSED---------------------------"<< endl;
            cout << timeNow() << endl;

            break;
        }
    }
}


char * IrcBot::timeNow()
{//returns the current date and time
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    return asctime (timeinfo);
}


bool IrcBot::sendData(string msg)
{// String sendData interface
    return sendData((char*)msg.c_str());
}


bool IrcBot::sendData(char *msg)
{//Send some data (deprecated)
    int len = strlen(msg);
    int bytes_sent = send(s,msg,len,0);

    if (bytes_sent == 0)
        return false;
    else
        return true;
}


void IrcBot::sendPong(string data)
{/* Pings must be replied with pongs or the connection will be
  * closed. See http://www.irchelp.org/irchelp/rfc/chapter4.html
  * for details
  */
    if (data == "")
        sendData((char*)"PONG\r\n");
    else
        sendData("PONG " + data + "\n");
    return;
}


int IrcBot::msgParse(string buf, string& sender, string& message)
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
    
    if (str.find("NOTICE AUTH") == 0)
    {
        message = str;
        return -1;
        //break;
    }
    
    // Parse message code
    intCode = atoi(str.substr(0,3).c_str());
    str = str.substr(4, str.size() - 4);
    
    // Remove receiver
    tmp = str.find(" ") + 1;
    message = str.substr(tmp, str.size() - tmp);
    
    return intCode;
}


void IrcBot::msgHandel(string buf)
{// Do stuff with messages
    int code;
    string message;
    string sender;
    
    // Parse message
    code = msgParse(buf, sender, message);
    
    // This could prove useful: http://tools.ietf.org/html/rfc1459.html
    switch (code)
    {

    // These messages need special treatment
    case -1:
        if (sender == "PING")
        {// It's a ping
            sendPong(message);
        } else {
            if (message.find("NOTICE AUTH") == 0)
            {// NOTICE AUTH contains the server name
                if (serverName == "server")
                    serverName = sender;
            }
            else
                cout<<message<<endl;
        }
        break;

    case 376: // MOTD is how we know we're connected
        cout<<"Joining "<<channelName<<"\n";
        sendData((char*)"JOIN " + channelName + "\r\n");
    case 375: // MOTD Header
    case 372: // MOTD Content
        break;

    case 0: // These messages are tricky and require a separate handler
        AI(sender, message);
        break;
    case 1: // This means we logged in successfully
        cout<<"Connection successful!\n";
        break;
    
    // No need to log this info
    case 2:
    case 3:
    case 4:
    case 5:
    case 251:
    case 252:
    case 254:
    case 255:
    case 265:
    case 266:
        break;
    
    // Channel nick list
    case 353:
    case 366:
        break;

    // Messages we don't yet handle will display on screen
    default:
        cout<<"<"<<sender<<"> ("<<code<<") "<<message<<endl;
    }
    return;
}

void IrcBot::AI(string sender, string msg)
{// Deal with humans
    string channel;
    string message;
    string name;
    string command;
    string args;
    int tmp;
    bool isAdmin = false;
    bool parameters;
    
    // Get sender's name
    name = sender.substr(0, sender.find("!"));
    if (name == nick) ; // IRC modes and stuff
    else
    {
        channel = msg.substr(0, msg.find(" "));
        message = msg.substr(channel.size() + 2,
            msg.size() - (channel.size() + 2));
        if (sender == "Synergiance!Syn@synerfiles.info")
            isAdmin = true;
        if (channel.substr(0,1) == "#")
        {// Message is a channel
            cout<<name<<" said on "<<channel<<": "<<message<<endl;

            if (message.find(nick + ": ") == 0)
            {// Got pinged, determine command
                if (message.size() > nick.size() + 2)
                {// Now we know there's text to parse after the ping
                    message = message.substr(nick.size() + 2,
                        message.size() - (nick.size() + 2));
                    if (extractCommandArgs(message, command, args))
                    {
                        // Print command in console
                        cout<<name<<" issued command "<<command<<" with "
                            <<(parameters ? ("parameters: " + message)
                            : "no parameters")<<endl;

                        // Commands go here now, so GIT
                        commandHandle(command, args, channelName, isAdmin);
                    }
                }
            }

            else if (message.find(nick) != -1) // Be nice
                say(channel, "Hi " + name);
        } else
        {// Message is a user
            cout<<name<<" said to me: "<<message<<endl;

            extractCommandArgs(message, command, args);

            // Commands go here now, now git
            commandHandle(command, args, name, isAdmin);
        }
    }
    return;
}

void IrcBot::say(string target, string message)
{
    cout<<"Saying ("<<target<<"): " + message<<endl;
    sendData("PRIVMSG " + target + " :" + message + "\r\n");
    return;
}

void IrcBot::action(string target, string message)
{
    /*
    string a = "\001";
    cout<<"Action ("<<target<<"): " + message<<endl;
    sendData("PRIVMSG " + target + " :" + a + "ACTION" + message + a + "\r\n");
    */
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
            for (long index = 0; (index<(long)quotes.size() && (alreadyTaken == false)); ++index)
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

void IrcBot::trimWhite(string& text)
{// Trim Whitespace
    while ((text.compare("") != 0) && (text.at(0) == ' '))
        text.erase(text.begin());
    while ((text.compare("") != 0) && (text.at(text.size() - 1) == ' '))
        text.erase(text.end() - 1);
    return;
}

int IrcBot::commandHandle(string cmd, string args, string talkto, bool admin)
{// This method handles all the commands sent to the bot
    int intReturn = 0;
    
    // Normal commands
    if (cmd.compare("add") == 0)
    {
        int tmpq;
        tmpq = addQuote(args);
        if (tmpq == -1)
        {
            cout<<"Quote already exists:\n"<<args<<endl;
            say(talkto, "Quote already exists");
        } else if (tmpq == 0) {
            cout<<"Adding quote:\n"<<args<<endl;
            say(talkto, "Quote added");
        } else {
            cout<<"Quote null\n";
            say(talkto, "Invalid quote: Null");
        }
    }
    if (cmd.compare("numquotes") == 0)
    {
        cout<<"There are "<<quotes.size()<<" quotes loaded\n";
        stringstream ss;
        ss<<"I have "<<quotes.size()<<" quotes loaded";
        say(talkto, ss.str());
    }

    // Say a random quote
    if (cmd.compare("quote") == 0)
    { if (quotes.size() > 0) {
        cout<<"Selecting random quote\n";
        say(talkto, quotes[rand() % quotes.size()]);
    } else
        say(talkto, "No quotes");
    }
    
    // Admin commands
    if (admin)
    {
        if (cmd.compare("say") == 0 && args.compare("") != 0)
        {
            say(channelName, args);
        }
        if (cmd.compare("action") == 0 && args.compare("") != 0)
        {
            say(talkto, "ACTION is currently buggy");
            cout<<"ACTION buggy, alerting user\n";
            action(channelName, args);
        }
        if (cmd.compare("showquote") == 0)
        {
            int intTmp = atoi(args.c_str());
            if (intTmp > 0)
            {
                intTmp--;
                if (intTmp < quotes.size())
                {
                    cout<<"Reciting quote "<<intTmp<<endl;
                    say(talkto, quotes[intTmp]);
                }
                else
                {
                    stringstream ss;
                    cout<<"Value entered is greater than number of quotes\n";
                    ss<<"There are only "<<quotes.size()<<" quotes in database"
                      <<", please enter a value less than "<<quotes.size();
                    say(talkto, ss.str());
                }
            }
            else if (intTmp < 0)
            {
                cout<<"Value entered was not positive: "<<intTmp<<endl;
                say(talkto, "Please enter a number greater than zero");
            }
            else if (args.compare("0") == 0)
            {
                cout<<"Value entered was 0\n";
                say(talkto, "Please enter a number greater than zero");
            }
            else if (args.compare("101702100412530688") == 0)
            {
                cout<<"easter egg\n";
                say(talkto, "Let's try a number less than that");
            }
            else
            {
                cout<<"No number detected\n";
                say(talkto, "Usage: showquote <number>");
            }
        }
        if (cmd.compare("remquote") == 0)
        {
            cout<<"Unimplemented function\n";
            say(talkto, "Function not yet implemented");
        }
    }
    return intReturn;
}

bool IrcBot::extractCommandArgs(string message, string& command, string& args)
{
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