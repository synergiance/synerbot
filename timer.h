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

#include "cmutex.h"

using namespace std;

#ifndef TIMER_H_
#define TIMER_H_

class cTimer {
private:
    vector<thread> timers;

    void timerCmd(time_t delay, string cmd);
};

#endif /* TIMER_H_ */

cTimer* timer;

void delayedCommand(int delay, string command);
