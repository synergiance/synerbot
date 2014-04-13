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
{// Load default privleges file
    privFile = "admins.cfg";
    loadFile();
    addedUsers = false;
}

CPrivleges::CPrivleges(string file)
{// Load specified privleges file
    privFile = file;
    loadFile();
    addedUsers = false;
}

CPrivleges::~CPrivleges()
{
    saveFile();
}

bool CPrivleges::loadFile()
{// Loads all admin strings from file
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

int CPrivleges::saveFile()
{// Saves users buffer to file, returns 0 if successful
    int fState = 0;
    if (addedUsers)
    {// Check to see that the admin buffer isn't empty
        ofstream qfile (privFile.c_str(), ios::out | ios::trunc);
        if (qfile)
        {// Since the file opened, we can write to it
            cout<<"Writing "<<admins.size()<<" users to file...";
            for (long index = 0; index<(long)admins.size(); ++index)
                qfile<<admins.at(index)<<endl;
            qfile.close();
            cout<<" Done\n";
        } else {
            cout<<"File error, no users saved\n";
            fState = -1;
        }
    } else {
        cout<<"No buffered users to save\n";
        fState = 1;
    }
    return fState;
}

bool CPrivleges::checkUsr(string usr)
{// Checks whether a specified user is an admin
    bool bReturn = false;
    //cout<<"Testing: "<<usr<<endl;
    if (admins.size() == 0)
        bReturn = (toLower(usr).find(defAdmin) == 0);
    else
        for (int index = 0; bReturn == false && index<admins.size(); ++index)
            bReturn = (toLower(usr).find(admins[index]) == 0);
    return bReturn;
}

int CPrivleges::addUsr(string usr)
{// Adds a user the admins file and checls to see if it already exists
    bool alreadyTaken = false;
    trimWhite(usr);
    usr = toLower(usr);
    if (usr.compare("") != 0)
    {// Check to see the user exists
        if (admins.size() > 0)
            for (int index = 0; (index<admins.size() && (alreadyTaken == false)); ++index)
                if (usr.compare(admins.at(index)) == 0)
                    alreadyTaken = true;
        if (alreadyTaken == false)
        {// Add user
            admins.push_back(usr);
            addedUsers = true;
            saveFile();
            return 0;
        } else
            return -1;
    } else
        return -2;
}

int CPrivleges::remUsr(string usr)
{// Removes a user based on user string, calls integer version
    int intReturn = -2;
    trimWhite(usr);
    usr = toLower(usr);
    if (usr.compare("") != 0)
    {
        for (int index = 0; (intReturn == -2 && index<admins.size()); ++index)
            if (usr.compare(admins[index]) == 0)
                intReturn = remUsr(index);
    }
    else
        intReturn = -3;
    return intReturn;
}

int CPrivleges::remUsr(int pos)
{// Removes user at 0 based index, returns -1 if index is out of bounds
    int tmpRet = 0;
    if ((pos >= 0) && (pos < admins.size()))
    {
        admins.erase(admins.begin()+pos+1);
        addedUsers = true;
        saveFile();
    }
    else
        tmpRet = -1;
    return tmpRet;
}
