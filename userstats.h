/*
 * userstats.h
 *
 *  Created on 30 May 2014
 *      By: Tim Gerety
 */

// Global Includes
#include <string>
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

#endif /* CUSERSTATS_H_ */
