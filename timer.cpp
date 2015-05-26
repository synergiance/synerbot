/*
 * timer.cpp
 *
 *  Created on 5 March 2015
 *      By: Fiona (Synergiance)
 */

/*

This class is designed to do everything time related, including delayed actions

*/

// Global Includes
#include <time.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cctype>
#include <condition_variable>

// Local Includes
#include "timer.h"
#include "cmutex.h"

using namespace std;

cTimer::cTimer(CMutex& theQ)
{// Always initialize your variables to a value
    stopping = false;
    MessageQueue = &theQ;
}

cTimer::~cTimer()
{// Notify all threads to stop immediately
    stopping = true;
    cv.notify_all();
}

void cTimer::delayCommand(string duration, CMutexMessage event)
{// A command that can be delayed a certain amount of time in seconds
    thread(&cTimer::timerThread, this, duration, event).detach();
}

void cTimer::timerThread(string duration, CMutexMessage event)
{// The thread that will take care of said command
    unique_lock<mutex> lck(mtx);
    char a = duration.back();
    duration.pop_back();
    int b = atoi(duration.c_str());
    if (a == 'M')
        cv.wait_for(lck, chrono::minutes(b));
    else if (a == 'H')
        cv.wait_for(lck, chrono::hours(b));
    else if (a == 'S')
        cv.wait_for(lck, chrono::seconds(b));
    if (!stopping) MessageQueue->push(event);
}
