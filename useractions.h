/*
 * useractions.h
 *
 *  Created on 21 April 2014
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
    //virtual ~CConfig();

    // Configuration access methods
    string getNick();
    string getUsr();
    string getRealName();
    string getServer();
    string getServerName();
    string getChannelName();
    string getPort();
private:
    // Config variables
    string nick;
    string usr;
    string realName;
    string server;
    string cfgFile;
    string serverName;
    string channelName;
    string port;

    void loadConfig();
    void writeFillConfig();
};

#endif /* CONFIG_H_ */
