/**/
#include <iostream>
#include <string>

using namespace std;

class Servo{
public:
    enum TYPE_SERVO{ Base,Shoulder,Elbow,Wrist,Wrot,Grip };
    
    static Servo* getServoInstance(TYPE_SERVO);
    static const double ERROR = -1.0;
    int getRawData();
    string getName();
    void setAngle(double angle);
    double getAngle();
    string toString();
    
protected:
    int max, min, raw;
    double angle;
    string name;
    
    //overrided methods
    virtual void updateRawValue() = 0;
};
class BaseServo : public Servo{
public:
    BaseServo();
    void updateRawValue();
private:
    string TAG;
    int BASE_MIN;
    int BASE_MAX;
    double BASE_MIN_ANGLE;
    double BASE_MAX_ANGLE;
};

class ShoulderServo : public Servo{
public:
    ShoulderServo();
    void updateRawValue();
private:
    string TAG;
    int SHOULDER_MIN;
    int SHOULDER_MAX;
    double SHOULDER_MIN_ANGLE;
    double SHOULDER_MAX_ANGLE;
};
class ElbowServo : public Servo{
public:
    ElbowServo();
    void updateRawValue();
private:
    string TAG;
    int ELBOW_MIN;
    int ELBOW_MAX;
    double ELBOW_MIN_ANGLE;
    double ELBOW_MAX_ANGLE;
};
class WristServo : public Servo{
public:
    WristServo();
    void updateRawValue();
private:
    string TAG;
    int WRIST_MIN;
    int WRIST_MAX;
    double WRIST_MIN_ANGLE;
    double WRIST_MAX_ANGLE;
};
class WrotServo : public Servo{
public:
    WrotServo();
    void updateRawValue();
private:
    string TAG;
    int WROT_MIN;
    int WROT_MAX;
    double WROT_MIN_ANGLE;
    double WROT_MAX_ANGLE;
};
class GripServo : public Servo{
public:
    GripServo();
    void updateRawValue();
private:
    string TAG;
    int GRIP_MIN;
    int GRIP_MAX;
    double GRIP_MIN_LENGTH;
    double GRIP_MAX_LENGTH;
};
Servo* Servo::getServoInstance(TYPE_SERVO type){
    if(type == Base) return new BaseServo();
    if(type == Shoulder) return new ShoulderServo();
    if(type == Elbow) return new ElbowServo();
    if(type == Wrist) return new WristServo();
    if(type == Wrot) return new WrotServo();
    if(type == Grip) return new GripServo();
    return NULL;
}
void Servo::setAngle(double angle){
    this->angle = angle;
    this->updateRawValue();
}
double Servo::getAngle(){
    return this->angle;
}
int Servo::getRawData(){
    return raw;
}

string Servo::getName(){
    return name;
}
string Servo::toString(){
    return "Name:" + name + " " +
    "Max:" + to_string(max) + " " +
    "Min:" + to_string(min) + " " +
    "Angle:" + to_string(getAngle()) + " " +
    "Raw:" + to_string(getRawData());
}
BaseServo::BaseServo(){
    TAG = "BaseServo";
    name = TAG;
    setAngle(ERROR);
    BASE_MIN = 200;
    min = BASE_MIN;
    BASE_MAX = 850;
    max = BASE_MAX;
    BASE_MIN_ANGLE = -90.0;
    BASE_MAX_ANGLE = 90.0;
    raw = 512;
}
void BaseServo::updateRawValue(){
    double rate = (BASE_MAX - BASE_MIN)/(BASE_MAX_ANGLE - BASE_MIN_ANGLE);
    int amount = rate * (angle - BASE_MIN_ANGLE) + BASE_MIN;
    if(amount > BASE_MAX) amount = BASE_MAX;
    if(amount < BASE_MIN) amount = BASE_MIN;
    raw = amount;
}
ShoulderServo::ShoulderServo(){
    TAG = "ShoulderServo";
    name = TAG;
    setAngle(ERROR);
    SHOULDER_MIN = 210;
    min = SHOULDER_MIN;
    SHOULDER_MAX = 810;
    max = SHOULDER_MAX;
    SHOULDER_MIN_ANGLE = 0;
    SHOULDER_MAX_ANGLE = 180;
    raw = 212;
}
void ShoulderServo::updateRawValue(){
    double rate = (SHOULDER_MAX - SHOULDER_MIN)
    /(SHOULDER_MAX_ANGLE - SHOULDER_MIN_ANGLE);
    int amount = SHOULDER_MAX - rate * angle;
    if(amount > SHOULDER_MAX) amount = SHOULDER_MAX;
    if( amount < SHOULDER_MIN) amount = SHOULDER_MIN;
    raw = amount;
}
ElbowServo::ElbowServo(){
    TAG = "ElbowServo";
    name = TAG;
    setAngle(ERROR);
    ELBOW_MIN = 215;
    min = ELBOW_MIN;
    ELBOW_MAX = 825;
    max = ELBOW_MAX;
    ELBOW_MIN_ANGLE = 0;
    ELBOW_MAX_ANGLE = 180;
    raw = 212;
}
void ElbowServo::updateRawValue(){
    double rate = (ELBOW_MAX - ELBOW_MIN)/(ELBOW_MAX_ANGLE - ELBOW_MIN_ANGLE);
    int amount = ELBOW_MIN + rate * angle;
    if(amount > ELBOW_MAX) amount = ELBOW_MAX;
    if(amount < ELBOW_MIN) amount = ELBOW_MIN;
    raw = amount;
}
WristServo::WristServo(){
    TAG = "WristServo";
    name = TAG;
    setAngle(ERROR);
    WRIST_MIN = 200;
    min = WRIST_MIN;
    WRIST_MAX = 800;
    max = WRIST_MAX;
    WRIST_MIN_ANGLE = -90;
    WRIST_MAX_ANGLE = 90;
    raw = 852;
}
void WristServo::updateRawValue(){
    double rate = (WRIST_MAX - WRIST_MIN)/(WRIST_MAX_ANGLE - WRIST_MIN_ANGLE);
    int amount = rate * (angle - WRIST_MIN_ANGLE) + WRIST_MIN;
    if(amount > WRIST_MAX) amount = WRIST_MAX;
    if(amount < WRIST_MIN) amount = WRIST_MIN;
    raw = amount;
}
WrotServo::WrotServo(){
    TAG = "WrotServo";
    name = TAG;
    setAngle(ERROR);
    WROT_MIN = 0;
    min = WROT_MIN;
    WROT_MAX = 999;
    max = WROT_MAX;
    WROT_MIN_ANGLE = -143;
    WROT_MAX_ANGLE = 143;
    raw = 512;
}
void WrotServo::updateRawValue(){
    double rate = (WROT_MAX - WROT_MIN)/(WROT_MAX_ANGLE - WROT_MIN_ANGLE);
    int amount = WROT_MAX - (angle - WROT_MIN_ANGLE) * rate;
    if(amount > WROT_MAX) amount = WROT_MAX;
    if(amount < WROT_MIN) amount = WROT_MIN;
    raw = amount;
}
GripServo::GripServo(){
    TAG = "GripServo";
    name = TAG;
    setAngle(ERROR);
    GRIP_MIN = 0;
    min = GRIP_MIN;
    GRIP_MAX = 512;
    max = GRIP_MAX;
    GRIP_MIN_LENGTH = 0;
    GRIP_MAX_LENGTH = 30;
    raw = 256;
}
void GripServo::updateRawValue(){
    if(angle > GRIP_MAX_LENGTH) angle = GRIP_MAX_LENGTH;
    if(angle < GRIP_MIN_LENGTH) angle = GRIP_MIN_LENGTH;
    double rate = (GRIP_MAX - GRIP_MIN)/(GRIP_MAX_LENGTH - GRIP_MIN_LENGTH);
    int amount = angle * rate;
    if(amount > GRIP_MAX) amount = GRIP_MAX;
    if(amount < GRIP_MIN) amount = GRIP_MIN;
    raw = amount;
}