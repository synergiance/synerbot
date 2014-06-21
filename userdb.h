/*
 * userdb.h
 *
 *  Created on 20 June 2014
 *      By: Tim Gerety
 */

#include <string>
#include <vector>

using namespace std;

#ifndef CUSERDB_H_
#define CUSERDB_H_

struct memberEntry
{
    vector<string> nicks;
    vector<int> nickints;
    vector<string> users;
    vector<int> userints;
    vector<string> hosts;
    vector<int> hostints;
    vector<string> names;
    vector<int> nameints;
};

class CUserDB
{
public:
    // Constructors
    CUserDB();
    ~CUserDB();

    //string checkUser(string nick, string user, string host);
    string checkUser(string username);
    string spotUser(string username);
private:
    vector<memberEntry> members;
    string userdbfile;

    void readdb();
    void writedb();

    int checkUser(string nick, string user, string host);
    string compileUser(int num);
};

#endif /* CUSERDB_H_ */
