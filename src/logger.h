/*
 This file needs boost, so need to specify which dir to be included.
 
 for example:
    g++ -I/opt/local/include logger.cpp -o logger && ./logger
 */
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <string>
#include <time.h>

using namespace std;
namespace pt = boost::posix_time;

class Logger{
public:
    const static int INFO = 2;
    const static int DEBUG = 1;
    const static int ERROR = 0;
    Logger();
    void setDebugLevel(int level);
    void i(string msg);
    void d(string msg);
    void e(string msg);
private:
    int level;
    void print_current_time();
    void print_msg(string mesg);
};


//PUBLIC METHODS
Logger::Logger(){
    level = ERROR;
}
void Logger::setDebugLevel(int level){
    this->level = level;
}
void Logger::i(string msg){
    if(level == INFO)
        print_msg(msg);
}
void Logger::d(string msg){
    if(level >= DEBUG)
        print_msg(msg);
}
void Logger::e(string msg){
    if(level >= ERROR)
        print_msg(msg);
}

//PRIVATE METHODS
void Logger::print_current_time(){
    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
    cout << "[" << current_date_microseconds <<"] ";
}
void Logger::print_msg(string msg){
    print_current_time();
    cout << msg << endl;
}