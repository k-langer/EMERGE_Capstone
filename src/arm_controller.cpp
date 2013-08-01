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

#include "mysql_connection.h"
#include "mysql_driver.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
using namespace std;

string SERIAL_PORT = "/dev/tty.usbserial-A901G712";
string BAUD_RATE = "38400";
int INPUT_LENGTH = 31;
unsigned int SLEEP_TIME = 10000000; //ten second
unsigned int INIT_TIME = 10000000; //ten second
int NO = 0;
int YES = 1;

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

//main
int main(int argc, const char **argv){
    string control_input;
    string url(argc >= 2 ? argv[1] : EXAMPLE_HOST);
    const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
    const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
    const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);
    //open serial port
    int fd = open_port();
    if(fd < 0){
        cout << "Serial port is not open."<<endl;
        return 0;
    }
	
    //configure port
    set_interface_attribs (fd, B38400, 0);
    //set in/out permission
    set_blocking (fd, NO);
    cout << "Building serial connection with the robot arm."<<endl;
    usleep(INIT_TIME);
    cout << "Serial port connection is open and configured."<<endl;
    try {
        sql::mysql::MySQL_Driver * driver = sql::mysql::get_driver_instance();
        std::auto_ptr< sql::Connection > con(driver->connect(url, user, pass));
        con->setSchema(database);
        cout << "Database is connected."<<endl;

        std::auto_ptr< sql::Statement > stmt(con->createStatement());
        stmt->execute("CALL get_new_data(@rs)");

        std::auto_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT @rs AS _message"));
		
        while (res->next()) {
            control_input = res->getString("_message");
            if(control_input.length() > 0){
                process_input(fd, control_input);
            }else{
                cout << "No data is available."<<endl;
                cout << "Sleeping for " << SLEEP_TIME << endl;
                usleep(SLEEP_TIME);//sleep one second
            }
        }
        while(1){
            stmt->execute("CALL get_new_data(@rs);");
            res.reset(stmt->executeQuery("SELECT @rs AS _message;"));
            while (res->next()) {
                control_input = res->getString("_message");
                if(control_input.length() > 0){
                    process_input(fd, control_input);
                }else{
                    cout << "No data is available."<<endl;
                    cout << "Sleeping for " << SLEEP_TIME << endl;
                    usleep(SLEEP_TIME);//sleep one second
                }
            }
        }
    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
} 

void process_input(int fd, string input){
    string inputArray[8];
    int index = 0, delay_time;

    bool input_error = false;
    index = 0;
    input_error = false;
    for(int i = 0; i < 8; i++){
        inputArray[i] = "";
    }
    for(int i = 0; i < input.length(); i++){
        if(input.length() != 31){
            input_error = true;
            break;
        }
        if(input[i] != ' '){
            inputArray[index] += input[i];
        }else{
            index++;
            if(index == 8){
                cout << "too many argument"<<endl;
                input_error = true;
            }
        }
    }
    if(input.length() != INPUT_LENGTH){
        input_error = true;
    }
    if(input_error == false){
        cout << "passing values to serial port.."<<endl;
        for(int i = 0; i < 8; i++){
            cout << inputArray[i] << endl;
            toSerialPort(fd, inputArray[i]);
        }
        delay_time = atoi(inputArray[6].c_str())+10;
        delay_time *= 1000;
        cout << "sleeping:" << delay_time << endl;
        usleep(delay_time);
    }else{
        cout << "ignore this input: " << input << endl;
    }
}

void toSerialPort(int fd, string val){
    char n;
    fd_set rdfs;
    struct timeval timeout = {1, 0};
    char* temp;
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
  
