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
#include <unistd.h>

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
    void disconnect(string message);
    void disconnect();

    // Send data to thread
    void toThread(string data);
private:
    // Sockets
    int socket;  // The network
    int pNet[2]; // Pipe to program

    // Variables
    string svrAddress;
    string svrPort;
    string botNick;
    string botUser;

    bool sendData(char *msg);
    bool sendData(string msg);
    bool sendLine(string msg);
    void sendPong(string data);
    void wait(bool& isNet, string& data);

    void main();
};

#endif /* NETWORK_H_ */
