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

    string checkUser(string username);
    string checkUser(string nick, string user, string host, string name);
    string spotUser(string nick, string user, string host, string name);
    int searchUser(string nick, string user, string host, string name);

    string compileUser(int num);

    memberEntry getUser(int num);
private:
    vector<memberEntry> members;
    string userdbfile;

    void readdb();
    void writedb();

};

#endif /* CUSERDB_H_ */
