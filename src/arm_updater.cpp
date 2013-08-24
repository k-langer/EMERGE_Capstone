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

#include <boost/date_time/posix_time/posix_time.hpp>
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
namespace pt = boost::posix_time;

double SHOULDER_LENGTH = 146.5;
double ELBOW_LENGTH = 146.5;
double WRIST_LENGTH = 100.0;
double PI = 3.14159265358979323846;

int BASE_MIN = 200;
int BASE_MAX = 850;
double BASE_MIN_ANGLE = -90.0;
double BASE_MAX_ANGLE = 90.0;

int SHOULDER_MIN = 210;
int SHOULDER_MAX = 810;
double SHOULDER_MIN_ANGLE = 0.0;
double SHOULDER_MAX_ANGLE = 180.0;

int ELBOW_MIN = 215;
int ELBOW_MAX = 825;
double ELBOW_MIN_ANGLE = 0;
double ELBOW_MAX_ANGLE = 180;

int WRIST_MIN = 200;
int WRIST_MAX = 800;
double WRIST_MIN_ANGLE = -90;
double WRIST_MAX_ANGLE = 90;

int WROT_MIN = 0;
int WROT_MAX = 999;
double WROT_MIN_ANGLE = -143;
double WROT_MAX_ANGLE = 143;

int GRIP_MIN = 0;
int GRIP_MAX = 512;
double GRIP_MIN_LENGTH = 0;
double GRIP_MAX_LENGTH = 30;

unsigned int SLEEP_TIME = 100000;//ten seconds
int XYZ_INPUT_COUNT = 9;
int COMMAND_INPUT_LENGTH = 51;

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
void get_current_time();
int main(int argc, const char **argv){

    Cartesian c;
    string url(argc >= 2 ? argv[1] : EXAMPLE_HOST);
    const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
    const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
    const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);
    ArmAngle angles;
    string input_string;
    vector<string> strs;
    string command_str;
    double wristAngle, wristRotAngle, gripAngle, speed, fWait;
    try{
        sql::mysql::MySQL_Driver * driver = sql::mysql::get_driver_instance();
        std::auto_ptr< sql::Connection > con(driver->connect(url, user, pass));
        con->setSchema(database);
        get_current_time();
        cout << "Database is connected."<<endl;
        
        std::auto_ptr< sql::Statement > stmt(con->createStatement());
        std::auto_ptr< sql::ResultSet > res;
        int total = 0;
        int successful = 0;
        int no_solution = 0;
        string tmp;
        while(1){
            stmt->execute("CALL get_xyz(@rs)"); 
            
            res.reset(stmt->executeQuery("select @rs as _val"));
            while (res->next()) {
                tmp = res->getString("_val");
                strs = split(tmp, ' ');
            }
            if(tmp.length() == 0) continue;

            get_current_time();
            cout << "Data available..." << endl;
            total += 1;    
            if(strs.size() != XYZ_INPUT_COUNT){
                get_current_time();
                cout << "Ignore this bad input:"<< emerge(strs, ' ') << endl;
                continue;
            }
            get_current_time();
            cout << "processing input:" << emerge(strs, ' ') << endl;


            command_str = strs[0];
            c.x = atof(strs[1].c_str());
            c.y = atof(strs[2].c_str());
            c.z = atof(strs[3].c_str());
            wristAngle = atof(strs[4].c_str());
            wristRotAngle = atof(strs[5].c_str());
            gripAngle = atof(strs[8].c_str());
            speed = atoi(strs[6].c_str());
            fWait = atoi(strs[7].c_str());
            angles = to_armangle(c, wristAngle, wristRotAngle, gripAngle);
            input_string = "CALL add_input('";
            input_string += command_str;
            input_string += " "; 
            input_string += input_composite(angles, speed, fWait);
            input_string += "')";
            if(angles.shoulder == -9999999) no_solution++;
            if(input_string.length() == COMMAND_INPUT_LENGTH){
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
                get_current_time();
                cout << "Successful: " << input_string << endl;
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
                stmt->execute(input_string);
                successful += 1;
            }else{
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
                get_current_time();
                cout << "Input ignore..."<< input_string << endl;
                cout << "Input Length:" << input_string.length() <<endl;
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
            }
            get_current_time();
            cout << "*******Rate:" << (double)successful/(double)total * 100 <<"% Sucessful:" << successful << " no_solution:" << no_solution << " total:" << total << "*****************"<<endl;
        }

        
    }catch(sql::SQLException &e){
         get_current_time();
         cout << "# ERR: SQLException in " << __FILE__;
         cout << "# ERR: " << e.what();
         cout << " (MySQL error code: " << e.getErrorCode();
         cout << ", SQLState: " << e.getSQLState() << " )" << endl;
         return EXIT_FAILURE;
    }
}
string input_composite(ArmAngle angles, int wTime, int fWait){
    string result = "";
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
    
    string t = to_string(wTime);

    while(t.length() < 4){
        t = "0" + t;
    }
    result += t;

    result += " ";
    result += to_string(fWait);

    return result;
}
Cartesian to_cart(ArmAngle arm){
    Cartesian c;
}

ArmAngle to_armangle(Cartesian c, double wristAngle, double wristRotAngle, double grip){
    cout << "~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    wristAngle = wristAngle * PI / 180;
    cout << "wrist angle:" << wristAngle << endl;
    ArmAngle angles;
    angles.wristRot = wristRotAngle;
    angles.grip = grip;
    
    //calculate the base and turn them into 2d plane problem
    angles.base = atan2(c.y, c.x);
    cout << "base:" << angles.base << endl;
    double axis_y = c.z;
    double axis_x = sqrt(pow(c.y, 2.0) + pow(c.x, 2.0));
    cout << "new y:" << axis_y << endl;
    cout << "new x:" << axis_x << endl;
    
    //now we can work on 2d plane
    double elbow_x = axis_x - WRIST_LENGTH * cos(wristAngle);
    double elbow_y = axis_y - WRIST_LENGTH * sin(wristAngle);
    cout << "elbow_x:" << elbow_x << endl;
    cout << "elbow_y:" << elbow_y << endl;

    //diagonal shoulder and elbow
    double dse = sqrt(pow(elbow_x, 2.0) + pow(elbow_y, 2.0));
    cout << "dse" << dse << endl;

    if(dse > SHOULDER_LENGTH + ELBOW_LENGTH){
        cout << "no solution" << endl; 
        cout << "SHOULDER_LENGTH:" << SHOULDER_LENGTH<<endl;
        cout << "ELBOW_LENGTH:" << ELBOW_LENGTH <<endl;
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
    else angles.elbow = atan2(sqrt(pow(temp2, 2) - pow(temp, 2)), temp) * 180 / PI;
    temp = normalize(pow(SHOULDER_LENGTH, 2.0) + pow(dse, 2.0) - pow(ELBOW_LENGTH, 2.0));
    temp2 =  (2*SHOULDER_LENGTH*dse);
    angles.shoulder = atan2(sqrt(pow(temp2, 2) - pow(temp, 2)), temp);
    cout << "shoulder1:"<< angles.shoulder<<endl; 
    angles.shoulder += atan2(elbow_y, elbow_x);
    cout << "shoulder2:"<< angles.shoulder<<endl; 
    angles.base *= 180 / PI;
    angles.shoulder *= 180 / PI;
    angles.wrist = wristAngle * 180 / PI + 180 - angles.elbow - angles.shoulder;
    cout << "elbow:"<<angles.elbow<<endl;
    cout << "shoulder:"<< angles.shoulder<<endl; 
    cout << "wrist: " << angles.wrist << endl;
    cout << "Grip: " << angles.grip << endl;
    return angles;
}
string get_base(double angle){
    //per degree
    double rate = (BASE_MAX - BASE_MIN)/(BASE_MAX_ANGLE - BASE_MIN_ANGLE);
    int amount = rate * (angle - BASE_MIN_ANGLE) + BASE_MIN;
    if(amount > BASE_MAX || amount < BASE_MIN){
        cout << "base amount:" << amount << " Max:"<<BASE_MAX << " Min:" << BASE_MIN<< endl;
        return "";
    }
    string result = to_string(amount);
    while(result.length() < 3){
        result = "0" + result;
    }
    return result;
}
string get_shoulder(double angle){
    double rate = (SHOULDER_MAX - SHOULDER_MIN)
                  /(SHOULDER_MAX_ANGLE - SHOULDER_MIN_ANGLE);
    int amount = SHOULDER_MAX - rate * angle;
    if(amount > SHOULDER_MAX || amount < SHOULDER_MIN){
        cout << "Shoulder amount:" << amount << " Max:" << SHOULDER_MAX << " Min:" << SHOULDER_MIN <<endl;
        return "";
    }
    string result = to_string(amount);
    while(result.length() < 3){
        result = "0" + result;
    }
    return result;
}
string get_elbow(double angle){
    double rate = (ELBOW_MAX - ELBOW_MIN)/(ELBOW_MAX_ANGLE - ELBOW_MIN_ANGLE);
    int amount = ELBOW_MIN + rate * angle;
    if(amount > ELBOW_MAX || amount < ELBOW_MIN){
        cout << "Elbow amount:" << amount << " Max:" << ELBOW_MAX << " Min:" << ELBOW_MIN << endl;
        return "";
    }
    string result = to_string(amount);
    while(result.length() < 3){
        result = "0" + result;
    }
    return result;
}
string get_wrist(double angle){
    double rate = (WRIST_MAX - WRIST_MIN)/(WRIST_MAX_ANGLE - WRIST_MIN_ANGLE);
    int amount = rate * (angle - WRIST_MIN_ANGLE) + WRIST_MIN;
    if(amount > WRIST_MAX || amount < WRIST_MIN) {
        cout << "Wrist amount:" << amount << " Max:" << WRIST_MAX << " Min:" << WRIST_MIN << endl;
        return "";
    }
    string result = to_string(amount);
    while(result.length() < 3){
        result = "0" + result;
    }
    return result;
}
string get_wrot(double angle){
    int amount;
    double rate = (WROT_MAX - WROT_MIN)/(WROT_MAX_ANGLE - WROT_MIN_ANGLE);
    amount = WROT_MAX - (angle - WROT_MIN_ANGLE) * rate;
    if(amount > WROT_MAX || amount < WROT_MIN) return "";
    if(amount > 999) amount = 999;
    string result = to_string(amount);
    while(result.length() < 3){
        result = "0" + result;
    }
    return result;
}
string get_grip(double length){
    int amount;
    if(length > GRIP_MAX_LENGTH) length = GRIP_MAX_LENGTH;
    if(length < GRIP_MIN_LENGTH) length = GRIP_MIN_LENGTH;
    double rate = (GRIP_MAX - GRIP_MIN)/(GRIP_MAX_LENGTH - GRIP_MIN_LENGTH);
    amount = length * rate;
    if(amount > GRIP_MAX || amount < GRIP_MIN) {
        cout << "Grip amout: " << amount << " Max:" << GRIP_MAX << " Min:" << GRIP_MIN << endl;   
        return "";
    }
    string result = to_string(amount);
    while(result.length() < 3){
        result = "0" + result;
    }
    return result;
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

void get_current_time(){
    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
    cout << "[" << current_date_microseconds <<"] ";
}
