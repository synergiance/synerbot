/*
 * cmutex.cpp
 *
 *  Created on 17 April 2014
 *      By: Tim Gerety
 */

// Global includes
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <condition_variable>
#include <chrono>

// Local includes
#include "cmutex.h"

using namespace std;

CMutex::CMutex()
{
    strBuffer.clear();
}

void CMutex::push(string str)
{// Pushes some data to the mutex buffer
    access(true, str);
}

bool CMutex::pull(string& str, int timeout) // Milliseconds
{// Blocking function that conditionally blocks
// Returns true if buffer is not empty
    if (timeout >= 0)
    {
        unique_lock<mutex> lck(mtx);
        if (timeout > 0)
            cv.wait_for(lck,chrono::milliseconds(timeout));
        else
            cv.wait(lck);
    }
    return access(false, str);
}

bool CMutex::access(bool adding, string& str)
{// Our mutex locked function, returns true if the buffer is not empty
    bool bReturn = false;
    mtx.lock();
    if (adding)
    {
        strBuffer.push_back(str);
        bReturn = true;
    }
    else
    {
        if (strBuffer.size() > 0)
        {
            str = strBuffer[0];
            strBuffer.erase(strBuffer.begin());
        }
        else
            str = "";
        if (strBuffer.size() > 0)
            bReturn = true;
    }
    mtx.unlock();
    return bReturn;
}
