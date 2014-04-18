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

// Local includes
#include "cmutex.h"

using namespace std;

CMutex::CMutex()
{
    strBuffer.clear();
}

void CMutex::push(string str)
{
    access(true, str);
}

string CMutex::pull()
{
    string str;
    access(false, str);
    return str;
}

void CMutex::access(bool adding, string& str)
{
    mtx.lock();
    if (adding)
        strBuffer.push_back(str);
    else
        if (strBuffer.size() > 0)
        {
            str = strBuffer[0];
            strBuffer.erase(strBuffer.begin());
        }
        else
            str = "";
    mtx.unlock();
}
