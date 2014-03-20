/**/
#include <iostream>
#include <string>
#include <cmath>
#include "cartesian.h"

using namespace std;
#ifndef __FORBIDDENZONE_H_INCLUDED__
#define __FORBIDDENZONE_H_INCLUDED__
class ForbiddenZone{
public:
    ForbiddenZone();
    bool contains(Cartesian);
private:
    Cartesian lowerCorner;
    Cartesian upperCorner;
};

ForbiddenZone::ForbiddenZone(){
    lowerCorner = Cartesian(20,20,0);
    upperCorner = Cartesian(-20,-20,110);
}

bool ForbiddenZone::contains(Cartesian input){
    if(input.getX() <= lowerCorner.getX() && input.getY() <= lowerCorner.getY()
       && input.getZ() >= lowerCorner.getZ() && input.getX() >= upperCorner.getX()
       && input.getY() >= upperCorner.getY() && input.getZ() <= upperCorner.getZ())
        return true;
    return false;
}
#endif