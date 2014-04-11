/*
 * config.cpp
 *
 *  Created on 15 February 2014
 *      By: Tim Gerety
 */

// Local Imports
#include "config.h"

// Global Imports
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

CConfig::CConfig()
{// Load default config file name
    loadConfig("ibot.cfg")
}

CConfig::CConfig(string file)
{// Load config file
    loadConfig(file)
}

void CConfig::loadConfig(string file)
{// Loads a configuration file
	//code
}