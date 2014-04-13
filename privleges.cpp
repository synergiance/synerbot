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

string defAdmin = "synergiance!syn@";

CPrivleges::CPrivleges()
{// Load default privleges file name
    privFile = "admins.cfg";
    loadFile();
}

CPrivleges::CPrivleges(string file)
{
    privFile = file;
    loadFile();
}

bool CPrivleges::loadFile()
{// Loads all admin strings from file
    // This is test code
    cout<<"This would open up the file: "<<privFile<<endl;
    // Look for file
    ifstream ifile (privFile.c_str());
    if (ifile)
    {// File exists, read configuration from it
        string strBuffer;
        cout<<"Found "<<privFile<<", loading admins... ";
        while (getline(ifile, strBuffer))
        {// Read a line
            trimWhite(strBuffer);
            // Insert string into vector if it isn't blank
            if (strBuffer.compare("") != 0)
                admins.push_back(strBuffer);
        }
        cout<<"Done!\n";
        // Always make sure to close your files
        ifile.close();
    } else {// File does not exist, create it and write defaults
        cout<<"Could not find "<<privFile<<", continuing with defaults\n";
    }
    return true;
}

bool CPrivleges::checkUsr(string usr)
{// Checks whether a specified user is an admin
    bool bReturn = false;
    //cout<<"Testing: "<<usr<<endl;
    if (admins.size() == 0)
        bReturn = (toLower(usr).find(defAdmin) == 0);
    else
        for (int index = 0; bReturn == false && index<admins.size(); index++)
            bReturn = (toLower(usr).find(admins[index]) == 0);
    return bReturn;
}
