/*
 * userdb.cpp
 *
 *  Created on 20 June 2014
 *      By: Tim Gerety
 */

/*

A class designed to keep track of users across hostnames

*/

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>
//#include <regex>

#include "userdb.h"
#include "miscbotlib.h"
#include "net.h"

using namespace std;

CUserDB::CUserDB()
{
    userdbfile = "userdb.db";
    readdb();
    debugMode = false;
}

CUserDB::~CUserDB()
{
    writedb();
}

void CUserDB::setDebug(bool mode)
{
    debugMode = mode;
}

string CUserDB::checkUser(string username)
{
    string nick, user, host; int tmp = -1;
    if (parseUser(username, nick, user, host))
        tmp = searchUser(nick, user, host, "");
    return compileUser(tmp);
}

string CUserDB::spotUser(string nick, string user, string host, string name)
{
    string ret = "";
    int num = searchUser(nick, user, host, name);
    if (num == -1) { // Create a new member
        memberEntry newMember;
        newMember.nicks.push_back(nick);
        newMember.nickints.push_back(1);
        newMember.users.push_back(user);
        newMember.userints.push_back(1);
        newMember.hosts.push_back(host);
        newMember.hostints.push_back(1);
        newMember.names.push_back(name);
        newMember.nameints.push_back(1);
        members.push_back(newMember);
        ret = nick + "!" + user + "@" + host;
    }
    else { // Add to existing member
        bool found = false;
        unsigned c;
        for (c = 0; c < members[num].nicks.size() && !found; c++)
            if (toLower(members[num].nicks[c]) == toLower(nick))
                found = true;
        if (found) {
            members[num].nickints[c-1]++;
        }
        else {
            members[num].nicks.push_back(nick);
            members[num].nickints.push_back(1);
        }
        found = false;
        for (c = 0; c < members[num].users.size() && !found; c++)
            if (toLower(members[num].users[c]) == toLower(user))
                found = true;
        if (found) {
            members[num].userints[c-1]++;
        }
        else {
            members[num].users.push_back(user);
            members[num].userints.push_back(1);
        }
        found = false;
        for (c = 0; c < members[num].hosts.size() && !found; c++)
            if (toLower(members[num].hosts[c]) == toLower(host))
                found = true;
        if (found) {
            members[num].hostints[c-1]++;
        }
        else {
            members[num].hosts.push_back(host);
            members[num].hostints.push_back(1);
        }
        found = false;
        for (c = 0; c < members[num].names.size() && !found; c++)
            if (toLower(members[num].names[c]) == toLower(name))
                found = true;
        if (found) {
            members[num].nameints[c-1]++;
        }
        else {
            members[num].names.push_back(name);
            members[num].nameints.push_back(1);
        }
        ret = compileUser(num);
    }
    return ret;
}

string CUserDB::compileUser(int num)
{// Returns a string representing the time a user has been seen
    string ret = "";
    if (num >= 0) ret = members[num].nicks[0] + "!" + members[num].users[0]
                      + "@" + members[num].hosts[0];
    return ret;
}

memberEntry CUserDB::getUser(int num)
{// Returns the user at specified index
    return members[num];
}

string CUserDB::checkUser(string nick, string user, string host, string name)
{// Checks the name on a user search
    return compileUser(searchUser(nick, user, host, name));
}

int CUserDB::searchUser(string nick, string user, string host, string name)
{// Searches and finds a user based on parameters
    int pos = -1;
    unsigned char score = 0;
    if (debugMode) {// Debug mode
        cout<<"Searching user database for:\nNick: "<<nick<<"\nUser: "
            <<user<<"\nHost: "<<host<<"\nName: "<<name<<endl;
    }
    if (members.size() > 0) {
        for (unsigned x = 0; x < members.size(); x++) {
            unsigned y;
            unsigned char tmpScore = 0;
            if (nick != "")
                for (y = 0; y < members[x].nicks.size(); y++)
                    if (toLower(members[x].nicks[y]) == toLower(nick))
                        tmpScore += 30;
            if (user != "")
                for (y = 0; y < members[x].users.size(); y++)
                    if (toLower(members[x].users[y]) == toLower(user))
                        tmpScore += 35;
            if (host != "")
                for (y = 0; y < members[x].hosts.size(); y++)
                    if (toLower(members[x].hosts[y]) == toLower(host))
                        tmpScore += 10;
            if (name != "")
                for (y = 0; y < members[x].names.size(); y++)
                    if (toLower(members[x].names[y]) == toLower(name))
                        tmpScore += 25;
            if (debugMode) {// Debug Mode
                cout<<compileUser(x)<<" "<<int(tmpScore)<<endl;
            }
            if (tmpScore > score) {
                score = tmpScore;
                pos = x;
            }
        }
        if (score < 45) pos = -1;
    }
    return pos;
}

posPair CUserDB::lookupUser(string nick, string user, string host, string name)
{// Searches and finds a user based on parameters
    posPair scores;
    if (debugMode) {// Debug mode
        cout<<"Searching user database for:\nNick: "<<nick<<"\nUser: "
            <<user<<"\nHost: "<<host<<"\nName: "<<name<<endl;
    }
    if (members.size() > 0) {
        for (unsigned x = 0; x < members.size(); x++) {
            unsigned y;
            unsigned char tmpScore = 0;
            if (nick != "")
                for (y = 0; y < members[x].nicks.size(); y++)
                    if (toLower(members[x].nicks[y]) == toLower(nick))
                        tmpScore += 30;
            if (user != "")
                for (y = 0; y < members[x].users.size(); y++)
                    if (toLower(members[x].users[y]) == toLower(user))
                        tmpScore += 35;
            if (host != "")
                for (y = 0; y < members[x].hosts.size(); y++)
                    if (toLower(members[x].hosts[y]) == toLower(host))
                        tmpScore += 10;
            if (name != "")
                for (y = 0; y < members[x].names.size(); y++)
                    if (toLower(members[x].names[y]) == toLower(name))
                        tmpScore += 25;
            if (debugMode) {// Debug Mode
                cout<<compileUser(x)<<" "<<int(tmpScore)<<endl;
            }
            if (tmpScore > 0) {
                scores.positions.push_back(x);
                scores.scores.push_back(tmpScore);
            }
        }
    }
    return scores;
}

void CUserDB::readdb()
{// Reads the user database into memory
    ifstream ifile (userdbfile.c_str());
    if (ifile) { // File exists, let's read from it
        string strBuffer;
        cout<<"Loading user database...";
        while (getline(ifile, strBuffer)) {
            if (strBuffer.compare("member {") == 0) { // Begin member section
                memberEntry newMember;
                while (getline(ifile, strBuffer)) {
                    if (strBuffer.compare("    nicks {") == 0) {
                        while (getline(ifile, strBuffer)) {
                            string str = strBuffer;
                            trimWhite(str);
                            if (str.compare("}") == 0) break;
                            if (str == "") continue;
                            int pos = str.rfind(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.nicks.push_back(str.substr(0, pos));
                            newMember.nickints.push_back(num);
                        }
                    }
                    else if (strBuffer.compare("    users {") == 0) {
                        while (getline(ifile, strBuffer)) {
                            string str = strBuffer;
                            trimWhite(str);
                            if (str.compare("}") == 0) break;
                            if (str == "") continue;
                            int pos = str.rfind(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.users.push_back(str.substr(0, pos));
                            newMember.userints.push_back(num);
                        }
                    }
                    else if (strBuffer.compare("    hosts {") == 0) {
                        while (getline(ifile, strBuffer)) {
                            string str = strBuffer;
                            trimWhite(str);
                            if (str.compare("}") == 0) break;
                            if (str == "") continue;
                            int pos = str.rfind(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.hosts.push_back(str.substr(0, pos));
                            newMember.hostints.push_back(num);
                        }
                    }
                    else if (strBuffer.compare("    names {") == 0) {
                        while (getline(ifile, strBuffer)) {
                            string str = strBuffer;
                            trimWhite(str);
                            if (str.compare("}") == 0) break;
                            if (str == "") continue;
                            int pos = str.rfind(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.names.push_back(str.substr(0, pos));
                            newMember.nameints.push_back(num);
                        }
                    }
                    else if (strBuffer.compare("}") == 0) break;
                }
                members.push_back(newMember);
            }
        }
        ifile.close();
        cout<<" Done!\n";
    }
    else {
        cout<<"No user database found\n";
    }
}

void CUserDB::writedb()
{
    if (members.size() > 0) {
        ofstream ofile (userdbfile.c_str());
        string tab = "    ";
        if (ofile) {
            cout<<"Saving user database...";
            for (unsigned c = 0; c < members.size(); c++) {
                ofile<<"member {\n"<<tab<<"nicks {\n";
                for (unsigned x = 0; x < members[c].nicks.size(); x++) {
                    ofile<<tab<<tab<<members[c].nicks[x]<<" "
                                   <<members[c].nickints[x]<<endl;
                }
                ofile<<tab<<"}\n"<<tab<<"users {\n";
                for (unsigned x = 0; x < members[c].users.size(); x++) {
                    ofile<<tab<<tab<<members[c].users[x]<<" "
                                   <<members[c].userints[x]<<endl;
                }
                ofile<<tab<<"}\n"<<tab<<"hosts {\n";
                for (unsigned x = 0; x < members[c].hosts.size(); x++) {
                    ofile<<tab<<tab<<members[c].hosts[x]<<" "
                                   <<members[c].hostints[x]<<endl;
                }
                ofile<<tab<<"}\n"<<tab<<"names {\n";
                for (unsigned x = 0; x < members[c].names.size(); x++) {
                    ofile<<tab<<tab<<members[c].names[x]<<" "
                                   <<members[c].nameints[x]<<endl;
                }
                ofile<<tab<<"}\n}\n\n";
            }
            ofile.close();
            cout<<" Done!\n";
        }
        else {
            cout<<"Error writing user database\n";
        }
    }
    else {
        cout<<"No users spotted\n";
    }
}

//------------------------------------------------------------------------------
//    Member Entry Code

string memberEntry::firstSeen()
{// Returns the first time the member has been seen by the bot
    stringstream ss;
    ss<<"First seen as: "<<nicks[0]<<"!"<<users[0]
      <<"@"<<hosts[0]<<" ("<<names[0]<<")";
    return ss.str();
}

string memberEntry::mostSeen()
{// Returns most seen variation of current member entry
    stringstream ss;
    string tmpstr;
    unsigned tmp = 0;
    ss<<"Most seen as: ";
    tmp = getHighest(nickints);
    tmpstr = nicks[tmp];
    ss<<tmpstr<<"!";
    tmp = getHighest(userints);
    tmpstr = users[tmp];
    ss<<tmpstr<<"@";
    tmpstr = string();
    getHighestMask(hostints, hosts, tmpstr);
    ss<<tmpstr<<" (";
    tmp = getHighest(nameints);
    tmpstr = names[tmp];
    ss<<tmpstr<<")";
    return ss.str();
}

string memberEntry::lastSeen()
{// Returns the last time that the member has been seen by the bot
    stringstream ss;
    ss<<"This function is not currently ready for production";
    return ss.str();
}

unsigned memberEntry::getHighest(const vector<int>& array)
{// Returns the entry with the most visability
    unsigned a = 0;
    for (unsigned c = 0; c < array.size(); c++)
        if (array[c] > array[a]) a = c;
    return a;
}

// Boiler Plate - (yes I love boiler plating things, sue me)
int memberEntry::getHighestMask (const vector<int>& stringNums,
    const vector<string>& strings, string& mask)
{ int a; return getHighestMask(stringNums, strings, mask, a); }

int memberEntry::getHighestMask (const vector<int>& stringNums,
    const vector<string>& strings, string& mask, int& num)
{// Create a wildcard mask for any string of characters
    int tmp = 0; string tmpstr;
    vector<string> masks;
    vector<int> maskNums;
    if (debugMode) cout<<"BEGIN SEARCH FOR HIGHEST MASK\n";
    if (stringNums.size() != strings.size()) return -1;
    if (debugMode) cout<<"Size: "<<strings.size()<<endl;
    for (unsigned x = 0; x < strings.size() - 1; x++) {
        if (debugMode) cout<<"Comparing: "<<strings[x]<<endl;
        for (unsigned y = x + 1; y < strings.size(); y++) {
            unsigned a, b; int d = 0; string str;
            if (debugMode) cout<<"To: "<<strings[y]<<endl;
            compare(strings[x], strings[y], a, b);
            if (debugMode) cout<<"Begin: "<<a<<" End: "<<b<<endl;
            if (a > 0 || b > 0) {
                if (a > 0) str += strings[x].substr(0,a);
                str += "*";
                if (b > 0) str += strings[x].substr(strings[x].size() - b);
                d = stringNums[x] + stringNums[y];
                if (debugMode) cout<<"Hypothetical mask: "<<str<<" "<<d<<endl;
            }
            if (d != 0) {
                for (unsigned e = 0; e < masks.size(); e++) {
                    if (str == masks[e]) {
                        str = ""; d = 0;
                        if (debugMode) cout<<"Duplicate\n";
                        break;
                    }
                }
                if (d > 0) {
                    d = 0;
                    for (unsigned e = 0; e < strings.size(); e++) {
                        unsigned f, g;
                        compare(str,strings[e],f,g);
                        if (f==a && g==b) d += stringNums[e];
                    }
                    masks.push_back(str);
                    maskNums.push_back(d);
                    if (debugMode) cout<<"Added\n";
                }
            }
        }
    }
    if (debugMode) cout<<"SEARCHING RAW\n";
    for (unsigned y = 0; y < strings.size(); y++) {
        if (debugMode) cout<<strings[y]<<" "<<stringNums[y];
        if (stringNums[y] > tmp) {
            tmp = stringNums[y];
            tmpstr = strings[y];
            if (debugMode) cout<<" √";
        }
        if (debugMode) cout<<endl;
    }
    if (debugMode) cout<<"SEARCHING GENERATED\n";
    for (unsigned y = 0; y < masks.size(); y++) {
        if (debugMode) cout<<masks[y]<<" "<<maskNums[y];
        if (maskNums[y] > tmp) {
            tmp = maskNums[y];
            tmpstr = masks[y];
            if (debugMode) cout<<" √";
        }
        if (debugMode) cout<<endl;
    }
    if (debugMode) cout<<"Final Result: "<<tmpstr<<endl;
    mask = tmpstr;
    num = tmp;
    return 0;
}

// Boiler Plate - (yes I love boiler plating things, sue me)
int memberEntry::getHighestHostMask (const vector<int>& stringNums,
    const vector<string>& strings, string& mask)
{ int a; return getHighestHostMask(stringNums, strings, mask, a); }

int memberEntry::getHighestHostMask (const vector<int>& stringNums,
    const vector<string>& strings, string& mask, int& num)
{// Makes a wildcard mask of hostnames/IPs
    vector< vector<string> > hosts;
    vector< vector<unsigned char> > IPv4hosts;
    vector< vector<int> > IPv6hosts;
    vector<int> hostNums;
    vector<int> IPv4hostNums;
    vector<int> IPv6hostNums;
    for (unsigned x = 0; x < strings.size(); x++) {
        string str = strings[x];
        size_t a; // This will be used everywhere anyways so don't waste bits
        if (check_IPv4(str)) {
            char num [3]; // Non-terminated character string of length 3
            unsigned char b, c, d, e;
            vector<unsigned char> IPv4hostBits;
            for (;;) {// More efficient than while (true)
                a = str.find('.'); e = 0;
                for (c = 0; c < 3; c++) num[c] = 0;
                if (a == string::npos) {
                    for (c = 0; c < 3 && c < str.length(); c++)
                        num[c] = str[str.length() - 1 - c];
                    break;
                } else {
                    for (c = 0; c < 3 && c < a; c++)
                        num[c] = str[a-1-c];
                    IPv4hostBits.push_back(e); str.erase(0,a+1);
                }
                for (c = 0; c < 3; c++) {// Base 10 convert without a null term
                    b = 1;
                    for (d = 0; d < c; d++) b *= 10;
                    e += b * (num[c] - 48);
                }
            }
            IPv4hosts.push_back(IPv4hostBits);
            IPv4hostNums.push_back(stringNums[x]);
        } else if (check_IPv6(str)) {
            a = str.find("::");
            size_t b;
            unsigned char c, d, e, f;
            int g;
            vector<int> IPv6hostBits;
            char tmpStr [5];
            if (a == string::npos) {// No shortening in address
                //code
            } else {// Address is shorthand, we'll have to do this in 2 parts
                // Initialize 8 ints in the array to 0
                for (c=0;c<8;c++) IPv6hostBits.push_back(0);
                for (c=0;c<8;c++) {// Iterator + limiter pretty much
                    b = str.find(":");
                    tmpStr = str.substr(0,b).c_str();
                    for (d=0;d<b;d++) {
                        g = 1; e = tmpStr[d] - 48;
                        if (e > 9) e -= 7; // A-F range
                        if (e > 9) e -= 32; // a-f range
                        for (f = 0; f < d; f++) g *= 16;
                    }
                }
            }
        } else {
            vector<string> hostBits;
            for (;;) {
                a = str.rfind('.');
                if (a == string::npos) {
                    hostBits.push_back(str);
                    break;
                } else {
                    hostBits.push_back(str.substr(a+1));
                    str = str.substr(0,a);
                }
            }
            hostNums.push_back(stringNums[x]);
            hosts.push_back(hostBits);
        }
    }
    return 0;
}
