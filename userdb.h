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

struct memberEntry {
public:
    vector<string> nicks;
    vector<int> nickints;
    vector<int> nicktime;
    vector<int> nickseen;
    vector<int> nickfrst;
    vector<string> users;
    vector<int> userints;
    vector<int> usertime;
    vector<int> userseen;
    vector<int> userfrst;
    vector<string> hosts;
    vector<int> hostints;
    vector<int> hosttime;
    vector<int> hostseen;
    vector<int> hostfrst;
    vector<string> names;
    vector<int> nameints;
    vector<int> nametime;
    vector<int> nameseen;
    vector<int> namefrst;

    string firstSeen();
    string mostSeen();
    string lastSeen();

    bool debugMode;
private:
    static const char* QuadOctetRegex;
    unsigned getHighest(const vector<int>& array);
    int getHighestMask (const vector<int>& stringNums,
        const vector<string>& strings, string& mask, int& num);
    int getHighestMask (const vector<int>& stringNums,
        const vector<string>& strings, string& mask);
    int getHighestHostMask (const vector<int>& stringNums,
        const vector<string>& strings, string& mask, int& num);
    int getHighestHostMask (const vector<int>& stringNums,
        const vector<string>& strings, string& mask);
};

struct posPair {
    vector<unsigned> positions;
    vector<unsigned char> scores;
};

class CUserDB
{
public:
    // Constructors
    CUserDB();
    ~CUserDB();

    void setDebug(bool mode);

    string checkUser(string username);
    string checkUser(string nick, string user, string host, string name);
    string spotUser(string nick, string user, string host, string name);
    int searchUser(string nick, string user, string host, string name);
    posPair lookupUser(string nick, string user, string host, string name);

    string compileUser(int num);

    memberEntry getUser(int num);
private:
    vector<memberEntry> members;
    string userdbfile;

    bool debugMode;

    void readdb();
    void writedb();

};

#endif /* CUSERDB_H_ */
