/**/
#include <iostream>
#include <string>
#include "arm.h"

using namespace std;

int main(){
    Arm* arm = new Arm();
    Cartesian* c = new Cartesian();
    c->setX(150);
    c->setY(160);
    c->setZ(170);
    arm->setPosition(*c);
    arm->setGripLength(20);
    arm->setWrotAngle(10);
    arm->setWristAngle(10);
    cout << arm->toString() << endl;
    
    arm->convertXYZToRaw();
    cout << arm->toString() << endl;
    
    cout << arm->toCommandString() << endl;
}