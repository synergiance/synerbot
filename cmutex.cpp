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
    //code
}

string CMutex::pull();
{
    //code
}

void CMutex::access(bool adding, string& str)
{
    //code
}
