/*
 * 8ball.h
 *
 *  Created on 12 February 2015
 *      By: Tim Gerety
 */

#include <string>
#include <vector>
#include <random>

using namespace std;

#ifndef C8BALL_H_
#define C8BALL_H_

class c8ball {
public:
    c8ball(CMutex& buffer, string chnl);

    bool loadstrings(string file);

    void getanswer();

    void say(string target, string message);

private:
    mt19937* rnd;
    CMutex *MessageQueue;
    bool loaded;
    string channel;

    vector<string> ballarray;
};

#endif /* C8BALL_H_ */
