/*
 * net.h
 *
 *  Created on 14 February 2014
 *      By: Tim Gerety
 */

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

#ifndef NETWORK_H_
#define NETWORK_H_

class NetShell
{
    //code
};

class NetSocket
{
public:
    // Class Functions
    NetSocket(string server, string port);
    virtual ~NetSocket();

    // Call Functions
    bool connect(string nick, string user);
private:
    bool sendData(char *msg);
    bool sendData(string msg);
    void sendPong(string data);
};

#endif /* NETWORK_H_ */