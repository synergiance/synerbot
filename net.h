/*
 * net.h
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
#include <mutex>
#include <thread>

// Local Includes
#include "cmutex.h"

// Obligatory namespace call
using namespace std;

#ifndef NETWORK_H_
#define NETWORK_H_

class CNetSocket
{
public:
    // Class Functions
    CNetSocket(string server, string port, CMutex& theQ);
    CNetSocket(string server, string port, CMutex& theQ, int debug);
    virtual ~CNetSocket();

    // Call Functions
    void botConnect(string nick, string user, string realName);
    void botDisconnect(string message);
    void botDisconnect();

    // Send data to thread
    void toThread(string data);

    bool setDisconnectMessage(string message);
private:
    // Sockets
    int sockfd;  // The network
    int pNet[2]; // Pipe to program

    int debugMode;

    // Working classes
    CMutex* MessageQueue;
    thread netThread;

    // Variables
    string svrAddress;
    string svrPort;
    string botNick;
    string botUser;
    string botRealName;
    string disconMessage;

    volatile bool isConnected; mutex mtxConnected;

    // Static Variables
    static string strDisconnected;

    bool accessConnected(int val);
    bool accessConnected();
    void accessConnected(bool val);

    int activateSocket();
    void killSocket();

    bool sendData(char *msg);
    bool sendData(string msg);
    bool sendLine(string msg);
    void sendPong(string data);
    bool wait(bool& bNet, bool& bPipe, string& strNet, string& strPipe);

    bool pipeHandle(string message);

    void main();

    void setup();

    void handleMessage(string data);
    void handleNumber(string sender, int code, string message);
};

#endif /* NETWORK_H_ */
