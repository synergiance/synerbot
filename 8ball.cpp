/*
 * 8ball.cpp
 *
 *  Created on 12 February 2015
 *      By: Tim Gerety
 */

// Loacal Includes
#include "cmutex.h"
#include "8ball.h"
#include "miscbotlib.h"

// Global Includes
#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <time.h>

using namespace std;

c8ball::c8ball(CMutex& buffer, string chnl)
{// Constructor
    unsigned rndseed = chrono::system_clock::now().time_since_epoch().count();
    char* rndmem = new char[sizeof(mt19937)];
    rnd = new (rndmem) mt19937(rndseed);

    loaded = loadstrings("8ball.lng");

    MessageQueue = &buffer;
    channel = chnl;
}

bool c8ball::loadstrings(string file)
{// Loads from the 8ball.lng file, returns true if successful
    bool fState = false;
    ifstream qfile (file.c_str());
    if (qfile)
    {// Determine if file exists
        string strInput;
        cout<<"Loading 8 ball strings from "<<file<<" into memory...\n";
        while (getline(qfile, strInput))
        {// Load a line
            trimWhite(strInput);
            // Insert string into vector if it isn't blank
            if (strInput.compare("") != 0)
                ballarray.push_back(strInput);
        }
        qfile.close();
        cout<<"Loaded "<<ballarray.size()<<" strings\n";
        fState = true;
    } else {
        cout<<file<<" does not exist, 8-ball not loaded.\n";
    }
    return fState;
}

void c8ball::getanswer()
{
    if (loaded)
    {// Only say a quote if we loaded the file
        uniform_int_distribution<int> dist(0, ballarray.size() - 1);
        int intTmp = dist(*rnd);
        say(channel, ballarray[intTmp]);
    }
}

void c8ball::say(string target, string message)
{
    MessageQueue->push("SAY " + target + " " + message);
}
