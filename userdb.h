/*
 * userdb.h
 *
 *  Created on 20 June 2014
 *      By: Synergiance
 */

#include <string>
#include <vector>
#include <bitset>

using namespace std;

#ifndef CUSERDB_H_
#define CUSERDB_H_

struct entry;

struct datalog {
    string title;
    int timesSeen;
    int lastSeen;
    int firstSeen;
    entry * link;
};

struct entry {
    string title;
    int timesSeen;
    int lastSeen;
    int firstSeen;
    vector<datalog> data1;
    vector<datalog> data2;
    vector<datalog> data3;
};


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

    void fuseWith(memberEntry food);

    bool debugMode;
private:
    unsigned getHighest(const vector<int>& array);
    int getHighestMask (const vector<int>& stringNums,
        const vector<string>& strings, string& mask, int& num);
    int getHighestMask (const vector<int>& stringNums,
        const vector<string>& strings, string& mask);
    int getHighestHostMask (const vector<int>& stringNums,
        const vector<string>& strings, string& mask, int& num);
    int getHighestHostMask (const vector<int>& stringNums,
        const vector<string>& strings, string& mask);

    bool IPv4parse(string str, unsigned char * array);
    bool IPv6parse(string str, vector<int>& array);
    bool DNSparse(string str, vector<string>& array);

    int IPv4search(const vector<int>& addrNums,
        const vector<unsigned char * >& addrs, string& mask, int& num);
    int IPv6search(const vector<int>& addrNums,
        const vector< vector<int> >& addrs, string& mask, int& num);
    int DNSsearch(const vector<int>& addrNums,
        const vector< vector<string> >& addrs, string& mask, int& num);

    string compileIPv4(unsigned char * nums);
    string compileIPv4(unsigned char * nums, bitset<4> mask);
    string compileIPv6(vector<int> nums);

    int quadhextoint(string hexNum);
    string inttoquadhex(int number);
    string numchartostring(unsigned char num);

    void mergeLists(vector<string>& meStrings,   vector<int>& meInts,
                    vector<string>& themStrings, vector<int>& themInts);
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
    posPair lookupUser(string query);

    string compileUser(int num);

    memberEntry getUser(int num);
private:
    vector<memberEntry> members;
    string userdbfile;

    bool debugMode;

    void readdb();
    void writedb();

    unsigned char scoreUsers(memberEntry& usr1, memberEntry& usr2);
    unsigned char scoreUser(memberEntry& usr, string test);
    int compareString(string str1, string str2);
};

#endif /* CUSERDB_H_ */
