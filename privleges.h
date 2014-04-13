/*
 * prifleges.h
 *
 *  Created on 11 April 2014
 *      By: Tim Gerety
 */

// Global Imports
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// Local Imports
#include "miscbotlib.h"

using namespace std;

#ifndef CPRIVLEGES_H_
#define CPRIVLEGES_H_

class CPrivleges
{
public:
    // Constructors
    CPrivleges();
    CPrivleges(string file);
    //virtual ~CPrivleges();

    // Calls
    bool checkUsr(string usr);
    int addUsr(string usr);
    int remUsr(string usr);
    int remUsr(int pos);

private:
    // Config variables
    string privFile;

    // Arrays
    vector<string> admins;

    // Variables
    bool addedUsers;

    bool loadFile();
    int saveFile();
};

#endif /* CPRIVLEGES_H_ */
