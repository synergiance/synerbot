/*
 *   Filename: posrgx.h
 *
 * Created on: 9 January 2016
 *     Author: Synergiance
 *
 * A compatible buffer between regex.h and c++ nicities, will act like <regex>
 */

#include <string>

bool     posrgx_match(string str1, string str2);
bool    posrgx_search(string str1, string str2);
string  posrgx_return(string str1, string str2);
string posrgx_replace(string str1, string str2, string str3);
