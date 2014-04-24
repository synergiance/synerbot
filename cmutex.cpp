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
    debugMode = false;
}

CMutex::CMutex(bool debug)
{
    strBuffer.clear();
    debugMode = debug;
}

void CMutex::push(string str)
{// Pushes some data to the mutex buffer
    unique_lock<mutex> lck(mtx2);
    //cout<<"Before Add Mutex\n";
    access(true, str);
    cv.notify_all();
}

bool CMutex::pull(string& str, int timeout) // Milliseconds
{// Blocking function that conditionally blocks
// Returns true if buffer is not empty
    //cout<<"Before before remove\n";
    if (timeout >= 0)
    {
        //cout<<"Before Unique lock\n";
        unique_lock<mutex> lck(mtx2);
        //cout<<"After Unique lock\n";
        if (timeout > 0)
            cv.wait_for(lck,chrono::milliseconds(timeout));
        else
            cv.wait(lck);
    }
    //cout<<"Before Remove Mutex\n";
    return access(false, str);
}

bool CMutex::access(bool adding, string& str)
{// Our mutex locked function, returns true if the buffer is not empty
    bool bReturn = false;
    //cout<<"MUTEX: Locking\n";
    mtx1.lock();
    //cout<<"MUTEX: Locked\n";
    if (adding)
    {
        //cout<<"Adding\n";
        strBuffer.push_back(str);
        bReturn = true;
        if (debugMode)
            cout<<"Adding: "<<str<<endl
                <<"Buffer size: "<<strBuffer.size()<<endl;
    }
    else
    {
        //cout<<"Checking\n";
        if (strBuffer.size() > 0)
        {
            //cout<<"Removing\n";
            str = strBuffer[0];
            strBuffer.erase(strBuffer.begin());
            if (debugMode)
                cout<<"Pulling: "<<str<<endl
                    <<"Buffer size: "<<strBuffer.size()<<endl;
        }
        else
            str = "";
        if (strBuffer.size() > 0)
            bReturn = true;
    }
    //cout<<"MUTEX: Unlocking\n";
    mtx1.unlock();
    //cout<<"MUTEX: Unlocked\n";
    return bReturn;
}
