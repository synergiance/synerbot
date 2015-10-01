/*
 * cmutex.h
 *
 *  Created on 17 April 2014
 *      By: Synergiance
 */

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <condition_variable>
#include <atomic>

using namespace std;

#ifndef CMUTEX_H_
#define CMUTEX_H_

struct CMutexMessage
{
public:
    string command;
    vector<string> command_arguments;
    string str();
    void clear();
};

class CMutex
{
public:
    CMutex();
    CMutex(bool debug);

    // Access array -- Deprecating
    void push(string str);
    bool pull(string& str);
    bool pull(string& str, int timeout);
    bool pull(string& str, int timeout, int delay);

    // Access array -- New
    void push(CMutexMessage str);
    bool pull(CMutexMessage& str);
    bool pull(CMutexMessage& str, int timeout);
    bool pull(CMutexMessage& str, int timeout, int delay);
private:
    // Array
    vector<CMutexMessage> strBuffer;

    bool debugMode;
    atomic_bool moreQuotes;

    // Mutex
    mutex mtx1, mtx2;
    condition_variable cv;

    // Blocked function
    bool access(bool adding, CMutexMessage& msg);
};

#endif /* CMUTEX_H_ */
