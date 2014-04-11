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
    virtual ~CConfig();

private:
    // Config variables
    string nick;
    string usr;
    string realName;
    string server;
    string cfgFile;
    string quoteFile;
    string serverName;
    string channelName;
    string port;

    // Read the file
    void loadConfig(string file);
};

#endif /* CONFIG_H_ */
