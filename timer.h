/*
 * timer.h
 *
 *  Created on 5 March 2015
 *      By: Fiona (Synergiance)
 */

#include <time.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <condition_variable>

#include "cmutex.h"

using namespace std;

#ifndef TIMER_H_
#define TIMER_H_



class cTimer {
public:
    cTimer(CMutex& theQ);
    ~cTimer();
    void delayCommand(string duration, CMutexMessage event);
private:
    bool stopping;
    condition_variable cv;
    mutex mtx;

    CMutex *MessageQueue;

    void timerThread(string duration, CMutexMessage event);
};

#endif /* TIMER_H_ */
