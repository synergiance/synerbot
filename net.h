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
    virtual ~CNetSocket();

    // Call Functions
    void botConnect(string nick, string user);
    void botDisconnect(string message);
    void botDisconnect();

    // Send data to thread
    void toThread(string data);
private:
    // Sockets
    int sockfd;  // The network
    int pNet[2]; // Pipe to program

    // Working classes
    CMutex* MessageQueue;
    thread netThread;

    // Variables
    string svrAddress;
    string svrPort;
    string botNick;
    string botUser;

    bool isConnected; mutex mtxConnected;

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
    void wait(bool& isNet, string& data);

    void main();

    void handleMessage(string data);
    void handleNumber(int code, string message);
};

#endif /* NETWORK_H_ */
