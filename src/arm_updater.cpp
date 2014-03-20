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
#include <fstream>
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

#include "pid_controller.h"
#include "logger.h"
#include "arm.h"
#include "helper.h"
#include "forbiddenZone.h"

using namespace std;
namespace pt = boost::posix_time;

#define HOST "localhost"
#define USER "zhen"
#define PASS "Zhenjiang#1"
#define DB "capstone"
#define CONTROLLING_DISABLE "99"

int MOVE_TIME = 7;
unsigned int SLEEP_TIME = 1000;//100 milliseconds
int XYZ_INPUT_COUNT = 9;
double PRECISION = 0.0000000001;
float PROPER_SPEED = 0.1;

vector<string> split(string str, char delimiter);
string emerge(vector<string> strs, char delimiter);

int main(int argc, const char **argv){
    Cartesian c;
    
    string url(HOST);
    const string user(USER);
    const string pass(PASS);
    const string database(DB);

    string input_string, tmp, ui_input;
    vector<string> strs;
    string command_str;
    int speed, fWait;
    float distance;
    Logger logger;
    logger.setDebugLevel(Logger::INFO);
    
    PID GripPID(GripServo::MAX, GripServo::MIN);
    PID WrotPID(WrotServo::MAX, WrotServo::MIN);
    PID WristPID(WristServo::MAX, WristServo::MIN);
    PID ElbowPID(ElbowServo::MAX, ElbowServo::MIN);
    PID ShoulderPID(ShoulderServo::MAX, ShoulderServo::MIN);
    PID BasePID(BaseServo::MAX, BaseServo::MIN);
    
    Arm *currArm = new Arm();
    Arm *prevArm = new Arm();
    ForbiddenZone forbiddenZone;
    
    try{
        //connect to database
        sql::Connection *con;
        sql::mysql::MySQL_Driver * driver;
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(url, user, pass);
        con->setSchema(database);
        logger.i("Database is connected");
        
        std::auto_ptr< sql::Statement > stmt(con->createStatement());
        std::auto_ptr< sql::ResultSet > res;

        while(1){
            stmt->execute("CALL get_xyz(@rs)"); 
            
            res.reset(stmt->executeQuery("select @rs as _val"));
            while (res->next()) {
                tmp = res->getString("_val");
                strs = split(tmp, ' ');
            }
            if(tmp.length() == 0){
                usleep(SLEEP_TIME);
                continue;
            }

            logger.i("Data available...");

            if(strs.size() != XYZ_INPUT_COUNT){
                logger.d("Ignore this bad input:" + emerge(strs, ' '));
                continue;
            }
            logger.i("processing input:" + emerge(strs, ' '));
            command_str = strs[0];
            
            if(command_str == CONTROLLING_DISABLE) {
                logger.i("Controlling is not enabled!");
                continue;
            }
            
            //set goal XYZ Position
            c.setX(atof(strs[1].c_str()) + 50);
            c.setY(atof(strs[2].c_str()));
            c.setZ(atof(strs[3].c_str()) - BASE_HEIGHT*2 - 50);
            
            if(c.getZ() < -50 || forbiddenZone.contains(c)){
                logger.d("Ignore input...");
                logger.d(c.toString());
                continue;
            }
            
            currArm->setPosition(c);
            currArm->setWristAngle(atof(strs[4].c_str()));
            currArm->setGripLength(atof(strs[8].c_str()));
            currArm->setWrotAngle(atof(strs[5].c_str()));
            if(!currArm->convertXYZToRaw()){
                logger.d("Input ignore...");
                logger.d(currArm->toString());
                continue;
            }
            ui_input = (currArm->getElbowXYZ()).toUICommandString();
            ui_input += " ";
            ui_input += (currArm->getWristXYZ()).toUICommandString();
            ui_input += " ";
            ui_input += c.toUICommandString();
            ui_input += " ";
            ui_input += to_string(currArm->getWrotAngle());
            ui_input += " ";
            ui_input += to_string(currArm->getGripLength());
            
            logger.i(currArm->toString());
            
            fWait = atoi(strs[7].c_str());
            
/*            //BasePID.setDebug(true);
            vector<int> baseStep = 
                BasePID.getSteps(prevArm->getBaseRaw(), currArm->getBaseRaw());
            vector<int> shoulderStep = 
                ShoulderPID.getSteps(prevArm->getShoulderRaw(), currArm->getShoulderRaw());
            vector<int> elbowStep = 
                ElbowPID.getSteps(prevArm->getElbowRaw(), currArm->getElbowRaw());
            vector<int> wristStep = 
                WristPID.getSteps(prevArm->getWristRaw(), currArm->getWristRaw());
            vector<int> wrotStep = 
                WrotPID.getSteps(prevArm->getWrotRaw(), currArm->getWrotRaw());
            vector<int> gripStep = 
                GripPID.getSteps(prevArm->getGripRaw(), currArm->getGripRaw());*/
            
            distance = c.distanceTo(prevArm->getCurrPosition());
            speed = distance / PROPER_SPEED;
            prevArm->setToArm(currArm);
//            for(int i = 0; i < baseStep.size(); i++){
                input_string = "CALL add_input('";
                input_string += command_str;
                input_string += Helper::formatString(to_string(currArm->getBaseRaw()),3);
                input_string += Helper::formatString(to_string(currArm->getShoulderRaw()),3);
                input_string += Helper::formatString(to_string(currArm->getElbowRaw()),3);
                input_string += Helper::formatString(to_string(currArm->getWristRaw()),3);
                input_string += Helper::formatString(to_string(currArm->getWrotRaw()),3);
                input_string += Helper::formatString(to_string(currArm->getGripRaw()),3);
                input_string += Helper::formatString(to_string(speed),4);
                input_string += Helper::formatString(to_string(fWait),1);
                input_string += "')";
                logger.i("Success:" + input_string);
                stmt->execute(input_string);
                stmt->execute("CALL add_ui_input('"+ui_input+"')");
  //          }
            usleep(SLEEP_TIME);
        }

        
    }catch(sql::SQLException &e){
        logger.e(e.what());
        return EXIT_FAILURE;
    }
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
