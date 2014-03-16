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
    void setRaw(int);
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
    static const int MIN = 200;
    static const int MAX = 850;
    static const double MIN_ANGLE = -90;
    static const double MAX_ANGLE = 90;
private:
    string TAG;
};

class ShoulderServo : public Servo{
public:
    ShoulderServo();
    void updateRawValue();
    static const int MIN = 210;
    static const int MAX = 810;
    static const double MIN_ANGLE = 0;
    static const double MAX_ANGLE = 180;
private:
    string TAG;
};
class ElbowServo : public Servo{
public:
    ElbowServo();
    void updateRawValue();
    static const int MIN = 215;
    static const int MAX = 810;
    static const double MIN_ANGLE = 0;
    static const double MAX_ANGLE = 180;
private:
    string TAG;
};
class WristServo : public Servo{
public:
    WristServo();
    void updateRawValue();
    static const int MIN = 200;
    static const int MAX = 800;
    static const double MIN_ANGLE = -90;
    static const double MAX_ANGLE = 90;
private:
    string TAG;
};
class WrotServo : public Servo{
public:
    WrotServo();
    void updateRawValue();
    static const int MIN = 0;
    static const int MAX = 999;
    static const double MIN_ANGLE = -143;
    static const double MAX_ANGLE = 143;
private:
    string TAG;
};
class GripServo : public Servo{
public:
    GripServo();
    void updateRawValue();
    static const int MIN = 0;
    static const int MAX = 512;
    static const double MIN_LENGTH = 0;
    static const double MAX_LENGTH = 30;
private:
    string TAG;
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
void Servo::setRaw(int raw){
    this->raw = raw;
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
    min = MIN;
    max = MAX;
    raw = 512;
}
void BaseServo::updateRawValue(){
    double rate = (MAX - MIN)/(MAX_ANGLE - MIN_ANGLE);
    int amount = rate * (angle - MIN_ANGLE) + MIN;
    if(amount > MAX) amount = MAX;
    if(amount < MIN) amount = MIN;
    raw = amount;
}
ShoulderServo::ShoulderServo(){
    TAG = "ShoulderServo";
    name = TAG;
    setAngle(ERROR);
    min = MIN;
    max = MAX;
    raw = 210;
}
void ShoulderServo::updateRawValue(){
    double rate = (MAX - MIN)
    /(MAX_ANGLE - MIN_ANGLE);
    int amount = MAX - rate * angle;
    if(amount > MAX) amount = MAX;
    if( amount < MIN) amount = MIN;
    raw = amount;
}
ElbowServo::ElbowServo(){
    TAG = "ElbowServo";
    name = TAG;
    setAngle(ERROR);
    min = MIN;
    max = MAX;
    raw = 215;
}
void ElbowServo::updateRawValue(){
    double rate = (MAX - MIN)/(MAX_ANGLE - MIN_ANGLE);
    int amount = MIN + rate * angle;
    if(amount > MAX) amount = MAX;
    if(amount < MIN) amount = MIN;
    raw = amount;
}
WristServo::WristServo(){
    TAG = "WristServo";
    name = TAG;
    setAngle(ERROR);
    min = MIN;
    max = MAX;
    raw = 710;
}
void WristServo::updateRawValue(){
    double rate = (MAX - MIN)/(MAX_ANGLE - MIN_ANGLE);
    int amount = rate * (angle - MIN_ANGLE) + MIN;
    if(amount > MAX) amount = MAX;
    if(amount < MIN) amount = MIN;
    raw = amount;
}
WrotServo::WrotServo(){
    TAG = "WrotServo";
    name = TAG;
    setAngle(ERROR);
    min = MIN;
    max = MAX;
    raw = 512;
}
void WrotServo::updateRawValue(){
    double rate = (MAX - MIN)/(MAX_ANGLE - MIN_ANGLE);
    int amount = MAX - (angle - MIN_ANGLE) * rate;
    if(amount > MAX) amount = MAX;
    if(amount < MIN) amount = MIN;
    raw = amount;
}
GripServo::GripServo(){
    TAG = "GripServo";
    name = TAG;
    setAngle(ERROR);
    min = MIN;
    max = MAX;
    raw = 256;
}
void GripServo::updateRawValue(){
    if(angle > MAX_LENGTH) angle = MAX_LENGTH;
    if(angle < MIN_LENGTH) angle = MIN_LENGTH;
    double rate = (MAX - MIN)/(MAX_LENGTH - MIN_LENGTH);
    int amount = angle * rate;
    if(amount > MAX) amount = MAX;
    if(amount < MIN) amount = MIN;
    raw = amount;
}