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

using namespace std;

#ifndef CMUTEX_H_
#define CMUTEX_H_

class CMutex
{
public:
    CMutex();
    
    // Access array
    void push(string str);
    string pull();
private:
    // Array
    vector<string> strBuffer;

    // Mutex
    mutex mtx;

    // Blocked function
    void access(bool adding, string& str);
};

#endif /* CMUTEX_H_ */
