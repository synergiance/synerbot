/*
 * miscbotlib.h
 *
 *  Created on 11 April 2014
 *      By: Tim Gerety
 */

#include <string>
#include <vector>

using namespace std;

#ifndef CMODULE_H_
#define CMODULE_H_

class CModule
{
public:
    // Constructors
    CModule();

    void getCapabilities(string& capabilities);
private:
};

#endif /* CMODULE_H_ */
