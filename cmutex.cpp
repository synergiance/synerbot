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
#include <atomic>

// Local includes
#include "cmutex.h"

using namespace std;

CMutex::CMutex()
{
    strBuffer.clear();
    debugMode = false;
    moreQuotes = false;
}

CMutex::CMutex(bool debug)
{
    strBuffer.clear();
    debugMode = debug;
    moreQuotes = false;
}

void CMutex::push(string str)
{// Pushes some data to the mutex buffer
    unique_lock<mutex> lck(mtx2);
    moreQuotes = access(true, str);
    cv.notify_all();
}

bool CMutex::pull(string& str)
{// Nonblocking pull
// Returns true if buffer is not empty
    return pull(str, -1, 0);
}

bool CMutex::pull(string& str, int timeout) // Milliseconds
{// Blocking function that conditionally blocks
// Returns true if buffer is not empty
    return pull(str, timeout, 0);
}

bool CMutex::pull(string& str, int timeout, int delay)
{// Blocking function that conditionally blocks
// Returns true if buffer is not empty
// timeout is in milliseconds
// delay is in microseconds
    if (timeout >= 0)
    {
        //if (!moreQuotes)
        {
            unique_lock<mutex> lck(mtx2);
            if (timeout > 0)
                cv.wait_for(lck,chrono::milliseconds(timeout));
            else
                cv.wait(lck);
        }
    }
    if (delay > 0) usleep(delay);
    moreQuotes = access(false, str);
    return moreQuotes;
}

bool CMutex::access(bool adding, string& str)
{// Our mutex locked function, returns true if the buffer is not empty
    bool bReturn = false;
    mtx1.lock();
    if (adding)
    {
        if (debugMode) cout<<"Adding: "<<str<<endl;
        try
        {
            strBuffer.push_back(str);
        }
        catch (const exception& e)
        {
            cerr<<"Error: "<<e.what()<<endl;
        }
        if (debugMode) cout<<"Buffer size: "<<strBuffer.size()<<endl;
    }
    else
    {
        if (strBuffer.size() > 0)
        {
            if (debugMode) cout<<"Pulling next\n";
            str = strBuffer[0];
            try
            {
            strBuffer.erase(strBuffer.begin());
            }
            catch (const exception& e)
            {
                cerr<<"Error: "<<e.what()<<endl;
            }
            if (debugMode)
                cout<<"Pulled: "<<str<<endl
                    <<"Buffer size: "<<strBuffer.size()<<endl;
        }
        else
        {
            str = ""; if (debugMode) cout<<"Pulled nothing, buffer clear\n";
        }
    }
    if (strBuffer.size() > 0)
        bReturn = true;
    mtx1.unlock();
    return bReturn;
}
