/*
 * net.cpp
 *
 *  Created on 14 February 2014
 *      By: Synergiance
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

#define MAXDATASIZE 128

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
    char* bfrmem = new char[sizeof(CMutex)];
    if (debugMode == 17 || debugMode == 18)
        PipeQueue = new (bfrmem) CMutex(true);
    else
        PipeQueue = new (bfrmem) CMutex();
    pipe(pNet);
    accessConnected(false);
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
        botNick = nick;
        botUser = user;
        botRealName = realName;
        if (netThread.joinable()) netThread.join();
        cout<<"Starting network...\n";
        netThread = thread(&CNetSocket::main, this);
        bufThread = thread(&CNetSocket::bufMain, this);
    }
}

void CNetSocket::botDisconnect(string message)
{// Tells the thread to hang up the call with specified message
    if (accessConnected())
    {
        toThread("net disconnect " + message);
    }
    if (netThread.joinable()) netThread.join();
    if (bufThread.joinable()) bufThread.join();
}

void CNetSocket::botDisconnect()
{// Tells the thread to hang up the call
    if (accessConnected())
    {
        toThread("net disconnect");
    }
    if (netThread.joinable()) netThread.join();
    if (bufThread.joinable()) bufThread.join();
}

void CNetSocket::toThread(string data)
{// Send some data to the thread
    //string str = data + "\r\n";
    //write(pNet[1], str.c_str(), str.size() + 1);
    //usleep(20); // Needs 20 microseconds or the pipe will get clogged
    PipeQueue->push(data);
}

void CNetSocket::bufMain()
{// This is a thread that will manage the pipe buffer
    string str, buf;
    bool keepRunning = true, moreBuffer = false;
    int delay = 5, minDelay = 5, maxDelay = 3000, maxCmdDelay = 50;

    while (keepRunning || moreBuffer)
    {// Stuff as many messages as you can into a single pipe send
        str = "";
        if (moreBuffer)
            moreBuffer = PipeQueue->pull(str); // No timeout or delay
        else
            moreBuffer = PipeQueue->pull(str, delay++ * 10, 10);
        if (delay > maxDelay) delay = maxDelay;
        if (str.compare("") == 0) continue;
        delay /= 4; if (delay < minDelay) delay = minDelay;
        if (delay > maxCmdDelay) delay = maxCmdDelay;
        if (toLower(str).find("net disconnect") == 0) keepRunning = false;
        if (toLower(str).find("net disconnected") == 0)
        { keepRunning = false; str = ""; } // Don't add this string to buffer
        buf += str + "\r\n";
        if (!moreBuffer && buf.compare("") != 0)
        {
            if (debugMode == 18) cout<<"Writing to pipe:\n"<<buf;
            write(pNet[1], buf.c_str(), buf.size() + 1); buf = "";
            usleep(100); // Needs some cool down time or the pipe will clog
        }
    }
    if (debugMode == 18) cout<<"Buffer killed\n";
}

void CNetSocket::main()
{// Contains the main loop for the CNetSocket class
    int tmp = activateSocket();
    bool keepGoing = false;
    string netBuffer, pipeBuffer;
    bool bNet, bPipe;

    if (debugMode == 14)
    {
        stringstream ss;
        ss<<"GLOBAL COUT Net Status: "<<tmp;
        MessageQueue->push(ss.str());
    }

    if (tmp == 0) {// Socket connected fine
        // Set all signals green
        keepGoing = true;
        accessConnected(true);
        // Send username info
        sendData("NICK " + botNick + "\r\n");
        sendData("USER " + botUser + " 8 * :" + botRealName + "\r\n");
    }
    /* --This section of code breaks ordering
    else if (tmp == -1) // We didn't manage to connect
        MessageQueue->push(strDisconnected);
    else // We may or may not want this special case
        MessageQueue->push(strDisconnected);
    */


    while (keepGoing)
    {// Main loop
        string strNet, strPipe, str; int found;
        bNet = false; bPipe = false;
        if (!wait(bNet, bPipe, strNet, strPipe)) keepGoing = false;
        if (bNet)
            netBuffer += strNet;
        if (bPipe)
            pipeBuffer += strPipe;

        if (bNet && strNet.compare(""))
            MessageQueue->push("GLOBAL NULLNET");

        if (!keepGoing) cout<<"Net disconnect\n";

        found = pipeBuffer.find('\n');
        while (found != -1)
        {// Separate messages in the pipe
            found++;
            str = pipeBuffer.substr(0, found);
            pipeBuffer = pipeBuffer.substr(found, pipeBuffer.size() - found);
            found = pipeBuffer.find('\n');
            if (str.find("\r\n") == string::npos || str.size() < 3) continue;
            str = str.substr(0, str.size() - 2);
            if (debugMode == 9) cout<<"PIPE: "<<str<<endl;
            if (debugMode == 10 || debugMode == 12)
                MessageQueue->push("COUT PIPE: " + str);
            if (!pipeHandle(str)) keepGoing = false;
        }

        found = netBuffer.find('\n');
        while (found != -1)
        {// Separate messages in the socket
            found++;
            str = netBuffer.substr(0, found);
            netBuffer = netBuffer.substr(found, netBuffer.size() - found);
            found = netBuffer.find('\n');
            if (str.find("\r\n") == string::npos || str.size() < 3) continue;
            str = str.substr(0, str.size() - 2);
            if (debugMode == 9) cout<<"NET: "<<str<<endl;
            if (debugMode == 10)
                MessageQueue->push("COUT NET: " + str);
            handleMessage(str);
        }
    }

    if (tmp == 0) {// Only run this if we were connected in the first place
        // Disconnect before we close
        MessageQueue->push("COUT Disconnecting...");
        if (disconMessage.compare("") != 0) {
            sendLine("QUIT :" + disconMessage);
            // Hold until closed connection or error
            shutdown(sockfd, 1); char buf[MAXDATASIZE];
            for(;;) if (recv(sockfd, buf, MAXDATASIZE, 0) <= 0) break;
        }
    } else {
        MessageQueue->push("COUT ERROR: Could not connect");
    }

    close(sockfd); // Close works even when not connected

    accessConnected(false);
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

    if (debugMode == 15)
    {
        stringstream ss;
        ss<<"COUT Sending ("<<bytes_sent<<"): "<<msg;
        MessageQueue->push(ss.str());
    }

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

    // Set to 1 second
    tv.tv_sec = 1;
    tv.tv_nsec = 0;

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
        } else bPipe = false;
        if (FD_ISSET(sockfd, &rfds))
        {// Pipe
            numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
            buf[numbytes] = '\0';
            strNet = buf;
            bNet = true;
        } else bNet = false;
    }
    return bReturn;
}

bool CNetSocket::accessConnected(int val)
{// Allows access to a mutex blocked boolean
    bool bReturn = true;
    mtxConnected.lock();
    if (val < 0)
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
    if (debugMode == 14) MessageQueue->push("COUT NET Acquired Address");
    if (s != 0) return -3;
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (debugMode == 14) MessageQueue->push("COUT NET Created Socket");
    if (sockfd == -1) return -2;
    s = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (debugMode == 14) MessageQueue->push("COUT NET Connect Success");
    if (s == -1) { close(sockfd); return -1; }

    // Import IP address
    char str[39];
    void *ptr;
    switch (res->ai_family)
    {
        case AF_INET:
            ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
            break;
        case AF_INET6:
            ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
            break;
    }
    inet_ntop(res->ai_family, ptr, str, 39);
    svrIP = str;

    freeaddrinfo(res);

    return 0;
}

bool CNetSocket::pipeHandle(string message)
{// Handles anything that comes into the pipe
    string cmd, args, str;

    // Filters
    if (!getFirstWord(message, cmd, str)) return true;
    if (debugMode == 11 || debugMode == 12)
        MessageQueue->push("COUT NET: PIPE HANDLER LAUNCHED");
    if (toLower(cmd).compare("net") != 0) return true;
    if (!getFirstWord(str, cmd, args)) return true;

    if (debugMode == 11 || debugMode == 12)
        MessageQueue->push("COUT NET: " + toUpper(cmd));

    if (toLower(cmd).compare("disconnect") == 0)
    {// Disconnect from server
        if (args.compare("") != 0)
            disconMessage = args;
        if (debugMode == 12)
            cout<<"NET: Disconnect message: "<<disconMessage<<endl;
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

    //cout<<"Test 1\n";

    // For debugging purposes
    if (debugMode == 6 || debugMode == 28) MessageQueue->push("COUT " + data);

    // Let's grab the first word
    if (!getFirstWord(data, sender, message)) return;

    //cout<<"Test 2\n";

    // Ping them back
    if (toUpper(sender).compare("PING") == 0)
        { sendPong(message); return; }

    // Grab second word
    if (!getFirstWord(message, command, str)) return;

    //cout<<"Test 3\n";

    // Grab numerical code
    code = atoi(command.c_str());

    //cout<<"Test 4\n";

    // Check numerical
    if (code > 0) // Use numerical handler
        handleNumber(sender, code, str);
    else // Send to main thread for processing
        MessageQueue->push("RAW " + data);

    //cout<<"Test 5\n";

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

    stringstream ss;

    // This could prove useful: http://tools.ietf.org/html/rfc1459.html
    switch (code)
    {

    case 1: // This means we logged in successfully (welcome)
        MessageQueue->push("GLOBAL CONNECTED");
        //cout<<"Connected\n";
        break;

    case 422: // MOTD file missing
    case 376: // MOTD Footer is how we know we're connected
        MessageQueue->push("GLOBAL MOTD");
        //cout<<"MOTD footer\n";
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
    case 252: // operators online
    case 253: // Unknown connections
    case 254: // I have x channels formed
    case 255: // I have x clients and y servers
    case 265: // Current local users
    case 266: // Current global users
    case 439: // Please wait while we process your connection (rizon)
        break;
    
    // Channel nick list
    case 353: // Lists all nicknames prefixed with their mode
        MessageQueue->push("GLOBAL NICKLIST "+data.substr(data.find(":")+1));
    case 366: // "End of /NAMES list."
        break;

    // Channel topic
    case 332: // Topic
    case 333: // User who set topic
        break;

    // Whois reply (all is prefixed with nickname)
    case 310: // is using modes [flags] authflags: [flags] (maybe op)
    case 311: // user information [nick] [user] [host] * :[realname]
    case 312: // server user is connected to [nick] [svrhost] :[svrdesc]
    case 317: // idle/signon [nick] [seconds] [time] :seconds idle, signon time
    case 318: // End of whois list
    case 319: // Channels user is connected to [nick] :[@#channel] <#channel2>
    case 338: // is actually [userhost] [IP] (yourself specific or op)
    case 672: // CGI:IRC client

    // yourself specific whois
    case 378: // Your host/IP
    case 379: // Your modes
        ss<<"WHOISRPLY "<<code<<" "<<data;
        MessageQueue->push(ss.str());
        break;

    // Messages we don't yet handle will display on screen
    default:
        if (debugMode == 4)
        {// For debugging purposes
            ss<<"COUT ("<<code<<") "<<data;
            MessageQueue->push(ss.str());
        }
        break;
    }
    return;
}

// Getter, nothing to see here, move along
string CNetSocket::getSvrIP() { return svrIP; }

/*
inet_pton(AF_INET, "192.0.2.1", &(sa.sin_addr)); // IPv4
inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr)); // IPv6
*/

bool check_IPv6(string addr)
{// Checks whether this is a valid IPv6 address
    struct sockaddr_in6 sa6;
    return inet_pton(AF_INET6, addr.c_str(), &(sa6.sin6_addr)) > 0;
}

bool check_IPv4(string addr)
{// Checks whether this is a valid IPv4 address
    struct sockaddr_in sa;
    return inet_pton(AF_INET, addr.c_str(), &(sa.sin_addr)) > 0;
}
