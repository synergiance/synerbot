/*
 * net.cpp
 *
 *  Created on 14 February 2014
 *      By: Tim Gerety
 */

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

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