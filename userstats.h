/*
 * userstats.h
 *
 *  Created on 30 May 2014
 *      By: Tim Gerety
 */

// Global Includes
#include <string>
#include <vector>

// Local Includes
#include "cmutex.h"

using namespace std;

#ifndef CUSERSTATS_H_
#define CUSERSTATS_H_

class CUserStats
{
public:
    // Constructors
    CUserStats();

private:
    CMutex* messageQueue;
};

struct CUserStatsUser
{
    vector<CUserStatsStat> stats;
    string username;

    int getStat(string statName);
};

struct CUserStatsStat
{
    string metricName;
    int value;
};

#endif /* CUSERSTATS_H_ */
