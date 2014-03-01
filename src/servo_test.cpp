/**/
#include <iostream>
#include <string>
#include "servo.h"

using namespace std;

int main(){
    
    Servo* baseServo = Servo::getServoInstance(Servo::Base);
    Servo* shoulderServo = Servo::getServoInstance(Servo::Shoulder);
    Servo* elbowServo = Servo::getServoInstance(Servo::Elbow);
    Servo* wristServo = Servo::getServoInstance(Servo::Wrist);
    Servo* wrotServo = Servo::getServoInstance(Servo::Wrot);
    Servo* gripServo = Servo::getServoInstance(Servo::Grip);
    
    cout << (baseServo->toString()) <<endl;
    cout << (shoulderServo->toString()) << endl;
    cout << (elbowServo->toString()) << endl;
    cout << (wristServo->toString()) << endl;
    cout << (wrotServo->toString()) << endl;
    cout << (gripServo->toString()) << endl;
    
    baseServo->setAngle(60);
    shoulderServo->setAngle(60);
    elbowServo->setAngle(60);
    wristServo->setAngle(60);
    wrotServo->setAngle(60);
    gripServo->setAngle(60);
    
    cout << (baseServo->toString()) <<endl;
    cout << (shoulderServo->toString()) << endl;
    cout << (elbowServo->toString()) << endl;
    cout << (wristServo->toString()) << endl;
    cout << (wrotServo->toString()) << endl;
    cout << (gripServo->toString()) << endl;
    
}