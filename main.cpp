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
#include <cstring>
#include <new>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>

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
            cout<<"\nDetected ctrl+c\n";
            bot->stop();
            //exit(signum);
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
    int debugMode = 0; bool verbose = false;
    
    for (int c = 1; c < argc; c++)
    {
        if (strncmp(argv[c], "-d", 20) == 0)
        {
            if (++c < argc)
            {
                debugMode = atoi(argv[c]);
                if (debugMode == 0 && strncmp(argv[c],"0",20) != 0)
                {
                    c--;
                    debugMode = 5;
                }
            }
            else
            {
                c--;
                debugMode = 5;
            }
        }
        if (strncmp(argv[c], "-v", 20) == 0)
            verbose = true;
    }

    // Register SIGINT signal
    signal(SIGINT, sigHandler);
    
    // Launch bot
    bot = new (botmem) IrcBot("ibot.cfg", debugMode, verbose);
    bot->start();
    

    return 0;

}
