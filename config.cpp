/*
 * config.cpp
 *
 *  Created on 15 February 2014
 *      By: Synergiance
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
    cfgFile = "ibot.cfg";
    loadConfig();
}

CConfig::CConfig(string file)
{// Load config file
    cfgFile = file;
    loadConfig();
}

void CConfig::loadConfig()
{// Loads a configuration file
    // Look for the file
    ifstream ifile (cfgFile.c_str());
    if (ifile)
    {// File exists, read configuration from it
        string strBuffer;
        cout<<"Loading configuration...";
        int tmp; string tmpstr; string tmpstr2;
        while (getline(ifile, strBuffer))
        {// Read a line
            tmp = strBuffer.find("=");
            unsigned int uTmp = tmp;
            if (tmp > -1 && (uTmp + 1) < strBuffer.length())
            {// Determine validity
                tmpstr = strBuffer.substr(0,tmp); tmp++;
                tmpstr2 = strBuffer.substr(tmp, strBuffer.length() - tmp);
                if (nick.compare("") == 0 && tmpstr.compare("Nick") == 0)
                    nick = tmpstr2;
                else if ((usr.compare("") == 0) &&
                         (tmpstr.compare("Username") == 0))
                    usr = tmpstr2;
                else if (realName.compare("") == 0 &&
                         tmpstr.compare("Description") == 0)
                    realName = tmpstr2;
                else if (server.compare("") == 0 &&
                         tmpstr.compare("Server") == 0)
                    server = tmpstr2;
                else if (serverName.compare("") == 0 &&
                         tmpstr.compare("ServerName") == 0)
                    serverName = tmpstr2;
                else if (port.compare("") == 0 &&
                         tmpstr.compare("Port") == 0)
                    port = tmpstr2;
                else if (channelName.compare("") == 0 &&
                         tmpstr.compare("Channel") == 0)
                    channelName = tmpstr2;
            }
        }
        cout<<" Done!\n";
        // Always make sure to close your files
        ifile.close();
    } else {// File does not exist, create it and write defaults
        cout<<"Using default configuration\n";
    }
    writeFillConfig();
}

bool CConfig::writeFillConfig()
{// Saves config to file and fills in missing defaults
    bool bSuccess;

    // Set defaults for missing config options
    if (nick.compare("") == 0)
        nick = "synerbot";
    if (usr.compare("") == 0)
        usr = "synerbot";
    if (realName.compare("") == 0)
        realName = "The bot of Synergiance";
    if (server.compare("") == 0)
        server = "127.0.0.1";
    if (serverName.compare("") == 0)
        serverName = "server";
    if (channelName.compare("") == 0)
        channelName = "#bots";
    if (port.compare("") == 0)
        port = "6667";

    // Let's write everything to disk
    ofstream ofile (cfgFile.c_str());
    if (ofile)
    {// Success
        cout<<"Writing configuration...";
        ofile<<"Nick="<<nick<<endl;
        ofile<<"Username="<<usr<<endl;
        ofile<<"Description="<<realName<<endl;
        ofile<<"Server="<<server<<endl;
        ofile<<"ServerName="<<serverName<<endl;
        ofile<<"Port="<<port<<endl;
        ofile<<"Channel="<<channelName<<endl;
        ofile.close(); cout<<" Done!\n";
        bSuccess = true;
    } else {// Errors
        cout<<"Error writing configuration\n";
        bSuccess = false;
    }
    return bSuccess;
}

// Configuration access methods
string CConfig::getNick()
{ return nick; }
string CConfig::getUsr()
{ return usr; }
string CConfig::getRealName()
{ return realName; }
string CConfig::getServer()
{ return server; }
string CConfig::getServerName()
{ return serverName; }
string CConfig::getChannelName()
{ return channelName; }
string CConfig::getPort()
{ return port; }
