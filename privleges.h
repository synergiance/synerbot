/*
 * prifleges.h
 *
 *  Created on 11 April 2014
 *      By: Tim Gerety
 */

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#ifndef CPRIVLEGES_H_
#define CPRIVLEGES_H_

class CPrivleges
{
public:
    // Constructors
    CPrivleges();
    CPrivleges(string file);
    virtual ~CPrivleges();

private:
    // Config variables
    string privFile;
};

#endif /* CPRIVLEGES_H_ */
