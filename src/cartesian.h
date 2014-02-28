/**/
#include <iostream>
#include <string>

using namespace std;

class Cartesian{
public:
    Cartesian();
    Cartesian(double x, double y, double z);
    void setX(double);
    void setY(double);
    void setZ(double);
    double getX();
    double getY();
    double getZ();
    void print();
private:
    double x;
    double y;
    double z;
};


//PUBLIC METHODS
Cartesian::Cartesian(){
    setX(0);
    setY(0);
    setZ(0);
}

Cartesian::Cartesian(double x, double y, double z){
    setX(x);
    setY(y);
    setZ(z);
}
void Cartesian::setX(double x){
    this->x = x;
}
void Cartesian::setY(double y){
    this->y = y;
}
void Cartesian::setZ(double z){
    this->z = z;
}
double Cartesian::getX(){
    return x;
}
double Cartesian::getY(){
    return y;
}
double Cartesian::getZ(){
    return z;
}
void Cartesian::print(){
    cout << "x:" << getX() << " "
         << "y:" << getY() << " "
         << "z:" << getZ() << endl;
}