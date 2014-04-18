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

// Local Includes
#include "net.h"
#include "cmutex.h"

using namespace std;

NetSocket::NetSocket(string server, string port);
{// Just prime the pumps with a server address and port
    svrAddress = server;
    svrPort = port;
}

NetSocket::~NetSocket()
{// This will close our socket when we kill the network class
    //code
}

int NetSocket::connect(string nick, string user)
{// This will open 
    int intReturn;
    intReturn = 0;
    return intReturn;
}

void NetSocket::disconnect(string message)
{// Tells the thread to hang up the call
    toThread("net disconnect" + message);
}

void NetSocket::disconnect()
{// Tells the thread to hang up the call
    toThread("net disconnect");
}

void NetSocket::toThread(string data)
{// Send some data to the thread
    //code
}

void NetSocket::main()
{// Contains the main loop for the NetSocket class
    //code
}

bool NetSocket::sendData(string msg)
{// String sendData interface
    return sendData((char*)msg.c_str());
}

bool NetSocket::sendData(char *msg)
{//Send some data (deprecated)
    int len = strlen(msg);
    int bytes_sent = send(s,msg,len,0);

    if (bytes_sent == 0)
        return false;
    else
        return true;
}

void NetSocket::sendPong(string data)
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

void NetSocket::wait(bool& isNet, string& data)
{// This will be doing some net magic
    //code
}
