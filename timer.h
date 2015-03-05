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

#include "cmutex.h"

using namespace std;

void delayedCommand(int delay, string command);

#ifndef TIMER_H_
#define TIMER_H_

class timer {
private:
    vector<thread> timers;
};

#endif /* TIMER_H_ */
