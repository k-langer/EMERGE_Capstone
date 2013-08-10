/**
  * To compile this file on Linux try something like:
  * g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include \
  * -lmysqlcppconn arm_updater.cpp 
  *
  * To run this code on Linux try something similar to:
  *
  *  LD_LIBRARY_PATH=/usr/local/lib/ ./a.out
  *
  * or:
  *
  *  LD_LIBRARY_PATH=/usr/local/lib/ ./a.out host user password database
  *
  */

#include <iomanip>
#include <iostream>
#include <string>
#include <math.h>
#include <time.h>
#include <termios.h>
#include <stdlib.h>
#include <vector>
#include "mysql_connection.h"
#include "mysql_driver.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


using namespace std;

float SHOULDER_LENGTH = 14.65;
float ELBOW_LENGTH = 14.65;
float WRIST_LENGTH = 14.65;
double PI = 3.14159265358979323846;

int BASE_MIN = 200;
int BASE_MAX = 850;
float BASE_MIN_ANGLE = -90.0;
float BASE_MAX_ANGLE = 90.0;

int SHOULDER_MIN = 210;
int SHOULDER_MAX = 810;
float SHOULDER_MIN_ANGLE = 0.0;
float SHOULDER_MAX_ANGLE = 180.0;

int ELBOW_MIN = 215;
int ELBOW_MAX = 825;
float ELBOW_MIN_ANGLE = 0;
float ELBOW_MAX_ANGLE = 180;

int WRIST_MIN = 200;
int WRIST_MAX = 800;
float WRIST_MIN_ANGLE = -90;
float WRIST_MAX_ANGLE = 90;

int WROT_MIN = 0;
int WROT_MAX = 1023;
float WROT_MIN_ANGLE = -90;
float WROT_MAX_ANGLE = 90;

int GRIP_MIN = 0;
int GRIP_MAX = 512;

unsigned int SLEEP_TIME = 10000000; //ten seconds
int XYZ_INPUT_COUNT = 8;

double PRECISION = 0.0000000001;
#define EXAMPLE_HOST "localhost"
#define EXAMPLE_USER "zhen"
#define EXAMPLE_PASS "Zhenjiang#1"
#define EXAMPLE_DB "capstone"
struct Cartesian{
        double x, y, z;
};

struct ArmAngle{
    double base, shoulder, elbow, wrist, wristRot, grip;
};

ArmAngle to_armangle(Cartesian, double, double, double);

string get_base(double);
string get_shoulder(double);
string get_elbow(double);
string get_wrist(double);
string get_wrot(double);
string get_grip(double);
string to_string(int number);
vector<string> split(string str, char delimiter);
string emerge(vector<string> strs, char delimiter);
string input_composite(ArmAngle, int wTime, int fWait);
Cartesian to_cart(ArmAngle);
double normalize(double);
int main(int argc, const char **argv){

    Cartesian c;
    string url(argc >= 2 ? argv[1] : EXAMPLE_HOST);
    const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
    const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
    const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);
    ArmAngle angles;
    string input_string;
    vector<string> strs;
    double wristAngle, wristRotAngle, gripAngle, speed, fWait;
    try{
        sql::mysql::MySQL_Driver * driver = sql::mysql::get_driver_instance();
        std::auto_ptr< sql::Connection > con(driver->connect(url, user, pass));
        con->setSchema(database);
        cout << "Database is connected."<<endl;
        
        std::auto_ptr< sql::Statement > stmt(con->createStatement());
        std::auto_ptr< sql::ResultSet > res;
        while(1){
            cout << "execute call get_xzy(@rs)"<<endl;
            stmt->execute("CALL get_xyz(@rs)"); 
            if(stmt->getUpdateCount() == 0){
                cout << "No data, so sleeping: "<< SLEEP_TIME/1000000 <<" seconds"<< endl;
                usleep(SLEEP_TIME);
                continue;
            }

            cout << "Data available..." << endl;

            res.reset(stmt->executeQuery("select @rs as _val"));
            while (res->next()) {
                strs = split(res->getString("_val"), ' ');
            }
            if(strs.size() != XYZ_INPUT_COUNT){
                cout << "Ignore this bad input:"<< emerge(strs, ' ') << endl;
                continue;
            }
            cout << "processing input:" << emerge(strs, ' ') << endl;

            c.x = atof(strs[0].c_str());
            c.y = atof(strs[1].c_str());
            c.z = atof(strs[2].c_str());
            wristAngle = atof(strs[3].c_str());
            wristRotAngle = atof(strs[4].c_str());
            gripAngle = atof(strs[5].c_str());
            speed = atoi(strs[6].c_str());
            fWait = atoi(strs[7].c_str());
            cout << c.x << " " << c.y << " " << c.z  << " " << wristAngle<< endl;
            angles = to_armangle(c, wristAngle, wristRotAngle, gripAngle);

            input_string = "CALL add_input('";
            input_string += input_composite(angles, speed, fWait);
            input_string += "')";
            cout << input_string << endl;
            stmt->execute(input_string);
        }

        
    }catch(sql::SQLException &e){
         cout << "# ERR: SQLException in " << __FILE__;
         cout << "# ERR: " << e.what();
         cout << " (MySQL error code: " << e.getErrorCode();
         cout << ", SQLState: " << e.getSQLState() << " )" << endl;
         return EXIT_FAILURE;
    }
}
string input_composite(ArmAngle angles, int wTime, int fWait){
    string result = "03";
    result += " ";
    result += get_base(angles.base);
    result += " ";
    result += get_shoulder(angles.shoulder);
    result += " ";
    result += get_elbow(angles.elbow);
    result += " ";
    result += get_wrist(angles.wrist);
    result += " ";
    result += get_wrot(angles.wristRot);
    result += " ";
    result += get_grip(angles.grip);
    result += " ";
    
    if(wTime > 999){
        result += to_string(wTime);
    }else{
        result += "0" + to_string(wTime);
    }
    result += " ";
    result += to_string(fWait);

    return result;
}
Cartesian to_cart(ArmAngle arm){
    Cartesian c;
}

ArmAngle to_armangle(Cartesian c, double wristAngle, double wristRotAngle, double grip){
    ArmAngle angles;
    wristAngle *= PI / 180;//convert to rad
    angles.wristRot = wristRotAngle;
    angles.grip = grip;
    
    //calculate the base and turn them into 2d plane problem
    angles.base = atan(c.y / c.x);
    double axis_y = c.z;
    double axis_x = sqrt(pow(c.y, 2.0) + pow(c.x, 2.0));
    
    //now we can work on 2d plane
    double elbow_x = axis_x - WRIST_LENGTH * cos(wristAngle);
    double elbow_y = axis_y - WRIST_LENGTH * sin(wristAngle);
    
    //diagonal shoulder and elbow
    double dse = sqrt(pow(elbow_x, 2.0) + pow(elbow_y, 2.0));
    if(dse > SHOULDER_LENGTH + ELBOW_LENGTH){
        angles.shoulder = -9999999;
        angles.elbow = -99999999;
        angles.wrist = -9999999;
        return angles;
    }
    
    //elbow angle using law of consines
    double temp = (pow(SHOULDER_LENGTH, 2.0) + pow(ELBOW_LENGTH, 2.0)
                                   - pow(dse, 2.0));
    double temp2 = 2*SHOULDER_LENGTH*ELBOW_LENGTH;
    
    //acos(1) == NaN causing problem
    if(abs((int)(temp/temp2)) == 1) angles.elbow = 0;
    else angles.elbow = acos((temp / temp2)) * 180 / PI;

    temp = normalize(pow(SHOULDER_LENGTH, 2.0) + pow(dse, 2.0) - pow(ELBOW_LENGTH, 2.0));
    temp2 = normalize(temp / (2*SHOULDER_LENGTH*dse));
    angles.shoulder = acos( temp2 );
    angles.shoulder += atan(elbow_y / elbow_x);
    
    angles.base *= 180 / PI;
    angles.shoulder *= 180 / PI;
    angles.wrist = wristAngle * 180 / PI + 180 - angles.elbow - angles.shoulder;
    cout << "wrist: " << angles.wrist << endl;
    return angles;
}
string get_base(double angle){
    //per degree
    double rate = (BASE_MAX - BASE_MIN)/(BASE_MAX_ANGLE - BASE_MIN_ANGLE);
    int amount = rate * (angle - BASE_MIN_ANGLE) + BASE_MIN;
    if(amount > BASE_MAX || amount < BASE_MIN)  return " ";
    return to_string(amount);
}
string get_shoulder(double angle){
    double rate = (SHOULDER_MAX - SHOULDER_MIN)
                  /(SHOULDER_MAX_ANGLE - SHOULDER_MIN_ANGLE);
    int amount = SHOULDER_MAX - rate * angle;
    if(amount > SHOULDER_MAX || amount < SHOULDER_MIN) return " ";
    return to_string(amount);
}
string get_elbow(double angle){
    double rate = (ELBOW_MAX - ELBOW_MIN)/(ELBOW_MAX_ANGLE - ELBOW_MIN_ANGLE);
    int amount = ELBOW_MIN + rate * angle;
    if(amount > ELBOW_MAX || amount < ELBOW_MIN) return " ";
    return to_string(amount);
}
string get_wrist(double angle){
    double rate = (WRIST_MAX - WRIST_MIN)/(WRIST_MAX_ANGLE - WRIST_MIN_ANGLE);
    int amount = rate * (angle - WRIST_MIN_ANGLE) + WRIST_MIN;
    if(amount > WRIST_MAX || amount < WRIST_MIN) return " ";
    return to_string(amount);
}
string get_wrot(double angle){
    int amount = 500;
    if(amount > WROT_MAX || amount < WROT_MIN) return " ";
    return to_string(amount);
}
string get_grip(double angle){
    int amount = 200;
    if(amount > GRIP_MAX || amount < GRIP_MIN) return " ";
    return to_string(200);
}
string to_string(int number){
    if (number == 0) return "0";
    string temp="";
    string returnvalue="";
    while (number>0){
        temp+=number%10+48;
        number/=10;   
    }
    for (int i=0;i<temp.length();i++)
        returnvalue+=temp[temp.length()-i-1];        
    return returnvalue;
}

vector<string> split(string str, char delimiter){
    vector<string> rs;
    string temp = "";
    //check str is not empty
    if(str.length() == 0) return rs;
    for(int i = 0; i < str.length(); i++){
        if(str[i] == delimiter){
            rs.push_back(temp);
            temp = "";
        }else{
            temp += str[i];
        }
    }
    rs.push_back(temp);
    return rs;
}

string emerge(vector<string> strs, char delimiter){
    string rs = "";
    if(strs.size() == 0) return rs;
    for(int i = 0; i < strs.size(); i++){
        rs += strs[i];
        if(i != strs.size() - 1) rs += delimiter;
    }
    return rs;
}

double normalize(double n){
    if(n < PRECISION) return 0;
    else return n;
}
