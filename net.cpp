/*
 * net.cpp
 *
 *  Created on 14 February 2014
 *      By: Tim Gerety
 */

// Global Includes
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/select.h>
#include <sys/time.h>

// Local Includes
#include "net.h"
#include "cmutex.h"
#include "miscbotlib.h"

using namespace std;

#define MAXDATASIZE 100

string CNetSocket::strDisconnected = "GLOBAL DISCONNECTED";

CNetSocket::CNetSocket(string server, string port, CMutex& theQ)
{// Just prime the pumps with a server address and port
    svrAddress = server;
    svrPort = port;
    debugMode = 0;
    MessageQueue = &theQ;
    disconMessage = "Watch out for them, they're coming for you!";
    setup();
}

CNetSocket::CNetSocket(string server, string port, CMutex& theQ, int debug)
{// Just prime the pumps with a server address and port
    svrAddress = server;
    svrPort = port;
    debugMode = debug;
    MessageQueue = &theQ;
    disconMessage = "Watch out for them, they're coming for you!";
    setup();
}

void CNetSocket::setup()
{// Prime defaults
    pipe(pNet);
    accessConnected(false);
    cout<<"Network setup complete\n";
}

CNetSocket::~CNetSocket()
{// This will close our socket when we kill the network class
    botDisconnect();
}

bool CNetSocket::setDisconnectMessage(string message)
{
    if (accessConnected())
        return false;
    else
        disconMessage = message;
    return true;
}

void CNetSocket::botConnect(string nick, string user, string realName)
{// This will open a socket and start the thread
    if (!accessConnected())
    {// Ignore any requests while connected, can only connect once
        botNick = nick; cout<<"Nick = "<<botNick<<endl;
        botUser = user; cout<<"User = "<<botUser<<endl;
        botRealName = realName;
        if (netThread.joinable()) netThread.join();
        cout<<"Creating thread...\n";
        netThread = thread(&CNetSocket::main, this);
        cout<<"Thread created!\n";
    }
}

void CNetSocket::botDisconnect(string message)
{// Tells the thread to hang up the call with specified message
    if (accessConnected())
    {
        toThread("net disconnect " + message);
        if (netThread.joinable()) netThread.join();
    }
}

void CNetSocket::botDisconnect()
{// Tells the thread to hang up the call
    if (accessConnected())
    {
        toThread("net disconnect");
        if (netThread.joinable()) netThread.join();
    }
}

void CNetSocket::toThread(string data)
{// Send some data to the thread
    string str = data + "\r\n";
    write(pNet[1], str.c_str(), str.size() + 1);
}

void CNetSocket::main()
{// Contains the main loop for the CNetSocket class
    int tmp = activateSocket();
    bool keepGoing = false;
    string netBuffer;
    string pipeBuffer;

    int numbytes;
    char buf[MAXDATASIZE];

    cout<<"THREAD: I'm alive\n";

    if (tmp == -1) // We didn't manage to connect
        MessageQueue->push(strDisconnected);
    else if (tmp == 0) // Socket connected fine
        keepGoing = true;
    else // We may or may not want this special case
        MessageQueue->push(strDisconnected);

    // Send username info
    sendData("NICK " + botNick + "\r\n");
    sendData("USER " + botUser + " 8 * :" + botRealName + "\r\n");

    while (keepGoing)
    {// Main loop
        bool bNet, bPipe; string strNet, strPipe, str; int found;
        if (!wait(bNet, bPipe, strNet, strPipe)) keepGoing = false;
        if (bNet)
            netBuffer += strNet;
        if (bPipe)
            pipeBuffer += strPipe;

        found = strPipe.find('\n');
        while (found != -1)
        {// Separate messages in the pipe
            found++;
            str = strPipe.substr(0, found);
            found = strPipe.find('\n');
            if (str.find("\r\n") == -1 || str.size() < 3) continue;
            str = str.substr(0, str.size() - 2);
            if (!pipeHandle(str)) keepGoing = false;
        }

        found = strNet.find('\n');
        while (found != -1)
        {// Separate messages in the socket
            found++;
            str = strNet.substr(0, found);
            found = strNet.find('\n');
            if (str.find("\r\n") == -1 || str.size() < 3) continue;
            str = str.substr(0, str.size() - 2);
            handleMessage(str);
        }
    }

    // Disconnect before we close
    if (disconMessage.compare("") != 0)
        sendLine("QUIT :" + disconMessage);
    close(sockfd);
    MessageQueue->push(strDisconnected);
}

bool CNetSocket::sendLine(string msg)
{// For people who are too lazy to put the CRLF chars on *raises hand*
    stringstream ss;
    ss<<msg<<"\r\n";
    return sendData((char*)ss.str().c_str());
}

bool CNetSocket::sendData(string msg)
{// String sendData interface
    return sendData((char*)msg.c_str());
}

bool CNetSocket::sendData(char *msg)
{//Send some data (deprecated)
    int len = strlen(msg);
    int bytes_sent = send(sockfd,msg,len,0);

    if (bytes_sent == 0)
        return false;
    else
        return true;
}

void CNetSocket::sendPong(string data)
{/* Pings must be replied with pongs or the connection will be
  * closed. See http://www.irchelp.org/irchelp/rfc/chapter4.html
  * for details
  */
    if (data == "")
        sendData((char*)"PONG\r\n");
    else
        sendData("PONG " + data + "\r\n");
    return;
}

bool CNetSocket::wait(bool& bNet, bool& bPipe, string& strNet, string& strPipe)
{// Wait on both the network and the pipe at once
    int numbytes;
    char buf[MAXDATASIZE];
    bool bReturn;

    // Stuff for select/pselect
    fd_set rfds; // Read file descriptor set
    struct timespec tv; // Timeout
    int retval; // Just a return value
    int mfd; // The highest fd

    FD_ZERO(&rfds); // Zero out the descriptor set
    FD_SET(pNet[0], &rfds); // Add pipe to descriptor set
    FD_SET(sockfd, &rfds); // Add network to descriptor set

    // Get higher file descriptor
    if (sockfd > pNet[0])
        mfd = sockfd + 1;
    else
        mfd = pNet[0] + 1;

    // Set to 1/100th of a second
    tv.tv_sec = 0;
    tv.tv_nsec = 10000;

    retval = pselect(mfd, &rfds, NULL, NULL, &tv, NULL);

    if (retval == -1)
        bReturn = false;
    else if (retval == 0)
        bReturn = true;
    else
    {// Read available sockets
        bReturn = true;
        if (FD_ISSET(pNet[0], &rfds))
        {// Network
            numbytes = read(pNet[0], buf, MAXDATASIZE - 1);
            buf[numbytes] = '\0';
            strPipe = buf;
            bPipe = true;
        }
        if (FD_ISSET(sockfd, &rfds))
        {// Pipe
            numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
            buf[numbytes] = '\0';
            strNet = buf;
            bNet = true;
        }
    }
    return bReturn;
}

bool CNetSocket::accessConnected(int val)
{// Allows access to a mutex blocked boolean
    bool bReturn = true;
    mtxConnected.lock();
    if (val >= 0)
        bReturn = isConnected;
    else
        if (val == 0)
            isConnected = false;
        else
            isConnected = true;
    mtxConnected.unlock();
    return bReturn;
}

bool CNetSocket::accessConnected()
{// Overloaded function
    return accessConnected(-1);
}

void CNetSocket::accessConnected(bool val)
{// Overloaded function
    if (val)
        accessConnected(1);
    else
        accessConnected(0);
}

int CNetSocket::activateSocket()
{// Start the network and return 0 if everything is fine
    struct addrinfo hints, *res; int s = 0;

    // Set up our hints variable
    memset(&hints, 0, sizeof hints); // Clear hints out with memset
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Use stream socket

    // Connect to the server
    s = getaddrinfo(svrAddress.c_str(), svrPort.c_str(), &hints, &res);
    if (s != 0) return -3;
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) return -2;
    s = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (s == -1) { close(sockfd); return -1; }

    freeaddrinfo(res);

    return 0;
}

bool CNetSocket::pipeHandle(string message)
{// Handles anything that comes into the pipe
    string cmd, args, str;

    // Filters
    if (!getFirstWord(message, cmd, str)) return true;
    if (toLower(cmd).compare("net") != 0) return true;
    if (!getFirstWord(str, cmd, args)) return true;

    if (toLower(cmd).compare("disconnect") == 0)
    {// Disconnect from server
        if (args.compare("") != 0)
            disconMessage = args;
        return false;
    }

    // Send a message
    if (toLower(cmd).compare("send") == 0)
        if (args.compare("") != 0)
            sendLine(args);

    return true;
}

void CNetSocket::handleMessage(string data)
{// At this point all the network fragmentation is gone
// See: http://tools.ietf.org/html/rfc1459.html
// See: https://tools.ietf.org/html/rfc2812

    // Variables
    string sender, command, message, str;
    int code = 0;

    // For debugging purposes
    if (debugMode == 6) MessageQueue->push("COUT " + data);

    // Let's grab the first word
    if (!getFirstWord(data, sender, message)) return;

    // Ping them back
    if (toUpper(sender).compare("PING") == 0)
        { sendPong(message); return; }

    // Grab second word
    if (!getFirstWord(message, command, str)) return;

    // Grab numerical code
    code = atoi(command.c_str());

    // Check numerical
    if (code > 0) // Use numerical handler
        handleNumber(sender, code, str);
    else // Send to main thread for processing
        MessageQueue->push("RAW " + data);

    return;
}

void CNetSocket::handleNumber(string sender, int code, string message)
{
    // Variables
    string receiver, data;

    // Let's get our other pieces or information
    getFirstWord(message, receiver, data);

    if (debugMode == 5)
    {// For debugging purposes
        stringstream ss;
        ss<<"COUT ("<<code<<") "<<data;
        MessageQueue->push(ss.str());
    }

    switch (code)
    {

    case 1: // This means we logged in successfully
        MessageQueue->push("GLOBAL CONNECTED");
        break;

    case 376: // MOTD Footer is how we know we're connected
        MessageQueue->push("GLOBAL MOTD");
    case 375: // MOTD Header
    case 372: // MOTD Content
        break;
    
    // Server connect messages
    case 2: // Server identity (with server type/version)
    case 3: // This server was created <time/date stamp>
    case 4: // Similar to 2
    case 5: // Server capabilities
    case 42: // Your unique ID (Inspircd)
    case 251: // Network info?
    case 252:
    case 254: // I have x channels formed
    case 255: // I have x clients and y servers
    case 265: // Current local users
    case 266: // Current global users
        break;
    
    // Channel nick list
    case 353: // Lists all nicknames prefixed with their mode
        MessageQueue->push("GLOBAL NICKLIST " + data);
    case 366: // "End of /NAMES list."
        break;

    // Channel topic
    case 332: // Topic
    case 333: // User who set topic
        break;

    // Messages we don't yet handle will display on screen
    default:
        if (debugMode == 4)
        {// For debugging purposes
            stringstream ss;
            ss<<"COUT ("<<code<<") "<<data;
            MessageQueue->push(ss.str());
        }
        break;
    }
    return;
}
