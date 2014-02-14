#!/bin/bash
HOME_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
src="$HOME_DIR/src"
des="$HOME_DIR/binary"
if [ $1 == "updater" ]
then
    echo "Compiling arm_updater..."
    g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include -lmysqlcppconn $src/arm_updater.cpp -o $des/updater
elif [ $1 == "controller" ]
then
    echo "Compiling arm_controller..."
    g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include -lmysqlcppconn $src/arm_controller.cpp -o $des/controller
elif [ $1 == "lm_data.java" ]
then
    echo "Compiling lm_data..."
    javac $src/lm_data.java -d $des/
else
    echo "Compiling arm_updater..."
    g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include -lmysqlcppconn $src/arm_updater.cpp -o $des/updater
    echo "Compiling arm_controller..."
    g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include -lmysqlcppconn $src/arm_controller.cpp -o $des/controller
    echo "Compiling lm_data..."
    javac $src/lm_data.java -d $des/

fi
