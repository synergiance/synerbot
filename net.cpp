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
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>

// Local Includes
#include "net.h"
#include "cmutex.h"

using namespace std;

CNetSocket::CNetSocket(string server, string port, CMutex& theQ)
{// Just prime the pumps with a server address and port
    svrAddress = server;
    svrPort = port;
    pipe(pNet);
    MessageQueue = &theQ;
    accessConnected(false);
}

CNetSocket::~CNetSocket()
{// This will close our socket when we kill the network class
    //code
}

void CNetSocket::botConnect(string nick, string user)
{// This will open a socket and start the thread
    if (!accessConnected())
    {// Ignore any requests while connected, can only connect once
        botNick = nick;
        botUser = user;
        if (netThread.joinable()) netThread.join();
        netThread = thread(&CNetSocket::main, this);
    }
}

void CNetSocket::botDisconnect(string message)
{// Tells the thread to hang up the call with specified message
    if (accessConnected())
    {
        toThread("net disconnect " + message);
        netThread.join();
    }
}

void CNetSocket::botDisconnect()
{// Tells the thread to hang up the call
    if (accessConnected())
    {
        toThread("net disconnect");
        netThread.join();
    }
}

void CNetSocket::toThread(string data)
{// Send some data to the thread
    //code
}

void CNetSocket::main()
{// Contains the main loop for the CNetSocket class
    int tmp = activateSocket();
    bool keepGoing = false;
    string disconMessage;

    int numbytes;
    char buf[MAXDATASIZE];

    if (tmp == -1) // We didn't manage to connect
        MessageQueue->push("GLOBAL DISCONNECTED");
    else if (tmp == 0) // Socket connected fine
        keepGoing = true;
    else // We may or may not want this special case
        MessageQueue->push("GLOBAL DISCONNECTED");

    while (keepGoing)
    {// Main loop
        //code
    }

    // Disconnect before we close
    if (disconMessage.compare("") != 0)
        sendLine("QUIT :" + disconMessage);
    close(sockfd);
    MessageQueue->push("GLOBAL DISCONNECTED");
}

bool sendLine(string msg)
{
    stringstream ss;
    ss<<msg<<"\r\n";
    sendData((char*)ss.str().c_str());
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
        sendData("PONG " + data + "\n");
    return;
}

void CNetSocket::wait(bool& isNet, string& data)
{// This will be doing some net magic
    //code
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
