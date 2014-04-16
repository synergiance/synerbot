/*
 * main.cpp
 *
 *  Created on 30 May 2013
 *      By: Synergiance
 */

#include <iostream>
#include <string>
#include <signal.h>
#include <cstdlib>
#include <new>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "bot.h"
#include "config.h"
#include "miscbotlib.h"


using namespace std;

//Global
IrcBot* bot;

void sigHandler(int signum)
{
    switch (signum)
    {
        // SIGINT
        case 2:
            cout<<"\nDetected ctrl+c, exiting...\n";
            bot->~IrcBot();
            exit(signum);
            break;

        // Mistake
        default:
            cout<<"\nUnknown signal detected...\n";
            break;
    }
    return;
}

int main(int argc, char* argv[])
{
    // Allocate memory
    char* botmem = new char[sizeof(IrcBot)];

    // Variables
    int debugMode = 0;
    
    for (int c = 1; c < argc; c++)
        if (argv[c] == "-d")
            if (++c < argc)
                if (debugMode = atoi(argv[c]) == 0 && argv[c] != "0")
                    c--;

    // Register SIGINT signal
    signal(SIGINT, sigHandler);
    
    // Launch bot
    bot = new (botmem) IrcBot("ibot.cfg", debugMode);
    bot->start();
    

    return 0;

}
