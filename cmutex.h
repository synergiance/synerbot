/*
 * cmutex.h
 *
 *  Created on 17 April 2014
 *      By: Tim Gerety
 */

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <condition_variable>

using namespace std;

#ifndef CMUTEX_H_
#define CMUTEX_H_

class CMutex
{
public:
    CMutex();

    // Access array
    void push(string str);
    bool pull(string& str, int timeout);
private:
    // Array
    vector<string> strBuffer;

    bool debugMode;

    // Mutex
    mutex mtx1, mtx2;
    condition_variable cv;

    // Blocked function
    bool access(bool adding, string& str);
};

#endif /* CMUTEX_H_ */
