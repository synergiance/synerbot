/*
 * userstats.h
 *
 *  Created on 30 May 2014
 *      By: Tim Gerety
 */

// Global Includes
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

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
    CUserStats(string file);
    //virtual ~CUserStats();

private:
};

class CUserStatsUser
{
public:
	// Constructors
	CUserStatsUser(string username);

	// Actions
	int addStat(string metric, int num);
	string enumMetrics();
	int getMetric(string metric);
	string getName();

private:
	vector<CUserStatsStat> stats;
	string username;
};

class CUserStatsStat
{
public:
	// Constructors
	CUserStatsStat(string metricName);

	int add(num);
	string getMetricName();
	int getValue();

private:
	string metricName;
	int value;
};

#endif /* CUSERSTATS_H_ */
