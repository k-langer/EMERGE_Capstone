/**
* To compile this file on Linux try something like:
* g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include \
* -lmysqlcppconn controller.cpp 
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

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "mysql_connection.h"
#include "mysql_driver.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
using namespace std;
namespace pt = boost::posix_time;
//string SERIAL_PORT = "/dev/tty.usbserial-A901G712";
string SERIAL_PORT = "/dev/ttyUSB0";
string BAUD_RATE = "38400";
unsigned int SLEEP_TIME = 10; //110 milliseconds
unsigned int INIT_TIME = 10000000; //ten second
int NO = 0;
int YES = 1;
int INPUT_LENGTH = 33;
int INPUT_COUNT = 9;
int DELAY_INDEX = 7;
string inputArray[33];

#define EXAMPLE_HOST "localhost"
#define EXAMPLE_USER "zhen"
#define EXAMPLE_PASS "Zhenjiang#1"
#define EXAMPLE_DB "capstone"

//function declaration
void toSerialPort(int, string);
int open_port(void);
int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
void process_input(int, string);
string get_time();
string int_to_string(int number);
void get_current_time();

bool debug = false;

//main
int main(int argc, const char **argv){
    string control_input;
    string url(EXAMPLE_HOST);
    const string user(EXAMPLE_USER);
    const string pass(EXAMPLE_PASS);
    const string database(EXAMPLE_DB);
    //open serial port
    int fd = open_port();
    if(fd < 0){
        get_current_time();
        cout << "Serial port is not open."<<endl;
        return 0;
    }
	
    //configure port
    set_interface_attribs (fd, B38400, 0);
    //set in/out permission
    set_blocking (fd, NO);
    get_current_time();
    cout << "Building serial connection with the robot arm."<<endl;
    usleep(INIT_TIME);
    get_current_time();
    cout << "Serial port connection is open and configured."<<endl;
    try {
        sql::mysql::MySQL_Driver * driver = sql::mysql::get_driver_instance();
        std::auto_ptr< sql::Connection > con(driver->connect(url, user, pass));
        con->setSchema(database);
        get_current_time();
        cout << "Database is connected."<<endl;

        std::auto_ptr< sql::Statement > stmt(con->createStatement());
        stmt->execute("CALL get_new_data(@rs)");

        std::auto_ptr< sql::ResultSet > 
            res(stmt->executeQuery("SELECT @rs AS _message"));
		
        while (res->next()) {
            control_input = res->getString("_message");
            if(control_input.length() > 0){
                process_input(fd, control_input);
            }
        }
        while(1){
            stmt->execute("CALL get_new_data(@rs);");
            res.reset(stmt->executeQuery("SELECT @rs AS _message;"));
            while (res->next()) {
                control_input = res->getString("_message");
                if(control_input.length() > 0){
                    get_current_time();
                    cout << "processing new data..."<< control_input << endl;
                    process_input(fd, control_input);
                    get_current_time();
                    cout << "write to serial port...done" << endl;
                }
            }
           // usleep(SLEEP_TIME);
        }
    } catch (sql::SQLException &e) {
        get_current_time();
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
} 

string get_time(){
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    string result = "";
    result += int_to_string(now->tm_mon + 1);
    result += "-";
    result += int_to_string(now->tm_mday);
    result += " ";
    result += int_to_string(now->tm_hour);
    result += ":";
    result += int_to_string(now->tm_min);
    result += ":";
    result += int_to_string(now->tm_sec);
    return result;
}
void process_input(int fd, string input){
    get_current_time();
    cout << "passing values to serial port.."<<endl;
    toSerialPort(fd, input);
    char response[1] = {'0'};
    get_current_time();
    cout << "Robot response:"<<endl;
    while(*response != '\t'){
        read (fd, response, sizeof response);
        if(*response == '\t') break;
        if(*response != '\n'){
            get_current_time();
            cout << response[0];
        }
        else
            cout << endl;
    }
    cout << endl;
    get_current_time();
    cout << "received all messages"<<endl;
}

void toSerialPort(int fd, string val){
    char n;
    fd_set rdfs;
    struct timeval timeout = {1, 0};
    char* temp;
    int timeout_count = 0;
    for(int i = 0; i < val.length(); i++){
        temp = &val[i];
        write(fd, temp, 1);
        FD_ZERO( &rdfs );
        FD_SET( fd, &rdfs );
        n = select(fd + 1, &rdfs, NULL, NULL, &timeout);
        if(n < 0){
            perror("select failed\n");
        }else if (n == 0){
            puts("Timeout!");
            timeout_count++;
        }else{
        }
    }
}
int open_port(void){
    int fd; // file description for the serial port
    fd = open(SERIAL_PORT.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd < 0){
        printf("open_port: Unable to open /dev/ttyS0. \n");
    }else{
        fcntl(fd, F_SETFL, 0);
        printf("port is open.\n");
    }
    return(fd);
}
int set_interface_attribs (int fd, int speed, int parity){
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0){
        printf("error %d from tcgetattr", errno);
        return -1;
    }
    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit char
    tty.c_iflag &= ~IGNBRK;         // ignore break signal
    tty.c_lflag = 0;                // no signaling chars, no echo,                                                                                                        // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeou
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    if (tcsetattr (fd, TCSANOW, &tty) != 0){
        printf("error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

void set_blocking (int fd, int should_block){
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0){
        printf("error %d from tggetattr", errno);
        return;
    }
    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
        printf("error %d setting term attributes", errno);
}
 string int_to_string(int number){
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

void get_current_time(){
    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
    cout << "[" << current_date_microseconds <<"] ";
}
