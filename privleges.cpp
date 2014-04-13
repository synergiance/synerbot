/*
 * privleges.cpp
 *
 *  Created on 11 April 2014
 *      By: Tim Gerety
 */

// Local Imports
#include "privleges.h"
#include "miscbotlib.h"

// Global Imports
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

string defAdmin = "synergiance!syn@"

CPrivleges::CPrivleges()
{// Load default privleges file name
    privFile = "admins.cfg";
}

CPrivleges::CPrivleges(string file)
{
    privFile = file;
}

bool CPrivleges::loadFile()
{// Loads all admin strings from file
    // This is test code
    cout<<"This would open up the file: "<<privFile<<endl;
}

bool CPrivleges::checkUsr(string usr)
{
    bool bReturn = false;
    if (admins.size() == 0)
        bReturn == (toLower(usr).find(defAdmin) == 0);
    else
        for (int index = 0; bReturn == false && index<admins.size(); index++)
            bReturn == (toLower(usr).find(admins[index]) == 0);
    return bReturn;
}
