/*
 * config.h
 *
 *  Created on 15 February 2014
 *      By: Tim Gerety
 */

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#ifndef CONFIG_H_
#define CONFIG_H_

class CConfig
{
public:
    // Constructors
    CConfig();
    CConfig(string file);
    ~CConfig();
};

#endif /* CONFIG_H_ */
