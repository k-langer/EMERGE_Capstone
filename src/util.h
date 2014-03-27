/**/
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;
#ifndef __UTIL_H_INCLUDED__
#define __UTIL_H_INCLUDED__
template <typename type>
string to_string(type val){
    stringstream ss (stringstream::in | stringstream::out);
    ss << val;
    return ss.str();
}
#endif
