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

#include "userdb.h"
#include "miscbotlib.h"

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
{
    string ret = "";
    if (num >= 0) ret = members[num].nicks[0] + "!" + members[num].users[0]
                      + "@" + members[num].hosts[0];
    return ret;
}

memberEntry CUserDB::getUser(int num)
{
    return members[num];
}

string CUserDB::checkUser(string nick, string user, string host, string name)
{
    return compileUser(searchUser(nick, user, host, name));
}

int CUserDB::searchUser(string nick, string user, string host, string name)
{// Searches and finds a user based on parameters
    int pos = -1;
    char score = 0;
    if (debugMode) {// Debug mode
        cout<<"Searching user database for:\nNick: "<<nick<<"\nUser: "
            <<user<<"\nHost: "<<host<<"\nName: "<<name<<endl;
    }
    if (members.size() > 0) {
        for (unsigned x = 0; x < members.size(); x++) {
            unsigned y;
            unsigned char tmpScore = 0;
            if (nick != "")
                for (y = 0; y < members[x].nicks.size() && pos == -1; y++)
                    if (toLower(members[x].nicks[y]) == toLower(nick))
                        tmpScore += 30;
            if (user != "")
                for (y = 0; y < members[x].users.size() && pos == -1; y++)
                    if (toLower(members[x].users[y]) == toLower(user))
                        tmpScore += 35;
            if (host != "")
                for (y = 0; y < members[x].hosts.size() && pos == -1; y++)
                    if (toLower(members[x].hosts[y]) == toLower(host))
                        tmpScore += 10;
            if (name != "")
                for (y = 0; y < members[x].names.size() && pos == -1; y++)
                    if (toLower(members[x].names[y]) == toLower(name))
                        tmpScore += 25;
            if (debugMode) {// Debug Mode
                cout<<compileUser(x)<<" "<<int(score)<<endl;
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
