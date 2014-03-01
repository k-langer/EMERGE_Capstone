/**/
#include <iostream>
#include <string>
#include <cmath>
#include "cartesian.h"
#include "servo.h"

using namespace std;

const long BASE_HEIGHT = 110L;
const long SHOULDER_LENGTH = 150L;
const long ELBOW_LENGTH = 147L;
const long WRIST_LENGTH = 137L;
const int COMMAND_INPUT_LENGTH = 51;
const double PI = 3.14159265358979323846;

class Arm{
public:
    Arm();
    
    //actions
    void setPosition(Cartesian c);
    void setGripLength(double gripLength);
    void setWrotAngle(double angle);
    void setWristAngle(double angle);
    
    //false if conversion failed;
    //need to call setPosition(), setGripLength(),
    //setWrotAngle()
    //this method calculates base, shoulder, wrist, elbow angles.
    bool convertXYZToRaw();
    
    Cartesian getCurrPosition();
    
    //servo values; needed to execute converXYZToRaw() first
    //return -1 if error
    int getBaseRaw();
    int getShoulderRaw();
    int getElbowRaw();
    int getWristRaw();
    int getWrotRaw();
    int getGripRaw();
    
    Cartesian getBaseXYZ();
    Cartesian getElbowXYZ();
    Cartesian getWristXYZ();
    double getWrotAngle();
    double getGripLength();
    
    string toString();
    string toCommandString();
private:
    Cartesian currcart;
    Cartesian baseXYZ;
    Cartesian elbowXYZ;
    Cartesian wristXYZ;
    
    Servo* baseServo;
    Servo* shoulderServo;
    Servo* elbowServo;
    Servo* wristServo;
    Servo* wrotServo;
    Servo* gripServo;
    
    bool validate();
    double radToAngle(double rad);
    double angleToRad(double angle);
    string formatString(string amount, int strlen);
};

Arm::Arm(){
    baseServo = Servo::getServoInstance(Servo::Base);
    shoulderServo = Servo::getServoInstance(Servo::Shoulder);
    elbowServo = Servo::getServoInstance(Servo::Elbow);
    wristServo = Servo::getServoInstance(Servo::Wrist);
    wrotServo = Servo::getServoInstance(Servo::Wrot);
    gripServo = Servo::getServoInstance(Servo::Grip);
}
//actions
void Arm::setPosition(Cartesian c){
    currcart = c;
}

Cartesian Arm::getCurrPosition(){
    return currcart;
}
void Arm::setGripLength(double gripLength){
    gripServo->setAngle(gripLength);
}
void Arm::setWrotAngle(double angle){
    wrotServo->setAngle(angle);
}
void Arm::setWristAngle(double angle){
    wristServo->setAngle(angle);
}
//servo values; needed to execute converXYZToRaw() first
//return -1 if error
int Arm::getBaseRaw(){
    return baseServo -> getRawData();
}
int Arm::getShoulderRaw(){
    return shoulderServo -> getRawData();
}
int Arm::getElbowRaw(){
    return elbowServo -> getRawData();
}
int Arm::getWristRaw(){
    return wristServo -> getRawData();
}
int Arm::getWrotRaw(){
    return wrotServo -> getRawData();
}
int Arm::getGripRaw(){
    return gripServo -> getRawData();
}

Cartesian Arm::getBaseXYZ(){
    return baseXYZ;
}
Cartesian Arm::getElbowXYZ(){
    return elbowXYZ;
}
double Arm::getWrotAngle(){
    return wrotServo -> getAngle();
}
double Arm::getGripLength(){
    return gripServo -> getAngle();
}

string Arm::toString(){
    return  "Current Position:" + getCurrPosition().toString() + "\n" +
            "Base XYZ:"+baseXYZ.toString() + "\n" +
            "Elbow XYZ:"+elbowXYZ.toString() + "\n" +
            "Wrist XYZ:"+wristXYZ.toString() + "\n" +
            "Servo stats:\n" +
            baseServo->toString() + "\n" +
            shoulderServo->toString() + "\n" +
            elbowServo->toString() + "\n" +
            wristServo->toString() + "\n" +
            wrotServo->toString() + "\n" +
            gripServo->toString() + "\n";
}
string Arm::toCommandString(){
    return  formatString(to_string(baseServo -> getRawData()), 3) +
            formatString(to_string(shoulderServo -> getRawData()), 3) +
            formatString(to_string(elbowServo -> getRawData()), 3) +
            formatString(to_string(wristServo -> getRawData()), 3) +
            formatString(to_string(wrotServo -> getRawData()), 3) +
            formatString(to_string(gripServo -> getRawData()), 3);
}

bool Arm::convertXYZToRaw(){
    if(wristServo->getAngle() == Servo::ERROR) return false;
    if(!currcart.getX() && !currcart.getY() && !currcart.getZ()) return false;
    
    //calculate the base
    baseServo->setAngle(radToAngle(atan2(currcart.getX(), currcart.getY())));
    
    //new x y plane
    double axis_y = currcart.getZ();
    double axis_x = sqrt(currcart.getX() * currcart.getX()
                         + currcart.getY() * currcart.getY());
    
    //now we can work on 2d plane
    double elbow_x = axis_x - WRIST_LENGTH *
                     cos(angleToRad(wristServo->getAngle()));
    double elbow_y = axis_y - WRIST_LENGTH *
                     sin(angleToRad(wristServo->getAngle()));
    
    //calculate elbow XYZ point
    wristXYZ.setX(elbow_x * cos(angleToRad(baseServo->getAngle())));
    wristXYZ.setY(elbow_x * sin(angleToRad(baseServo->getAngle())));
    wristXYZ.setZ(elbow_y);
    
    //diagonal from base point to elbow
    double dse = sqrt(elbow_x * elbow_x + elbow_y * elbow_y);
    if(dse > SHOULDER_LENGTH + ELBOW_LENGTH) return false;
    
    //elbow angle
    double temp = (pow(SHOULDER_LENGTH, 2.0) + pow(ELBOW_LENGTH, 2.0)
                   - pow(dse, 2.0));
    double temp2 = 2*SHOULDER_LENGTH*ELBOW_LENGTH;
    
    //acos(1) == NaN causing problem
    if(abs((int)(temp/temp2)) == 1) elbowServo->setAngle(0);
    else elbowServo->setAngle(radToAngle(atan2(sqrt(pow(temp2, 2) -
                                   pow(temp, 2)), temp)));
    
    temp = (pow(SHOULDER_LENGTH, 2.0) +
                     pow(dse, 2.0) - pow(ELBOW_LENGTH, 2.0));
    temp2 =  (2*SHOULDER_LENGTH*dse);
    double shoulderAngle = atan2(sqrt(pow(temp2, 2) - pow(temp, 2)), temp);
    shoulderAngle += atan2(elbow_y, elbow_x);
    shoulderServo->setAngle(radToAngle(shoulderAngle));
    
    //Calculate elbow xyz point
    elbowXYZ.setZ(SHOULDER_LENGTH*sin(angleToRad(shoulderServo->getAngle())));
    double xy = SHOULDER_LENGTH * cos(angleToRad(shoulderServo->getAngle()));
    elbowXYZ.setX(xy * cos(angleToRad(baseServo->getAngle())));
    elbowXYZ.setY(xy * sin(angleToRad(baseServo->getAngle())));
    
    
    //calculate wrist angles
    wristServo->setAngle(wristServo->getAngle() + 180
                        - elbowServo->getAngle() - shoulderServo->getAngle());
    return true;
}



//private methods
double Arm::radToAngle(double rad){
    return rad * 180 / PI;
}
double Arm::angleToRad(double angle){
    return angle * PI / 180;
}
string Arm::formatString(string amount, int strlen){
    string result = amount;
    while(result.length() < strlen){
        result = "0" + result;
    }
    return result;
}





