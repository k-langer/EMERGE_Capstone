#!/bin/bash
if [ $1 == "updater" ]
then
    echo "Compiling arm_updater..."
    g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include -lmysqlcppconn arm_updater.cpp -o updater
elif [ $1 == "controller" ]
then
    echo "Compiling arm_controller..."
    g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include -lmysqlcppconn arm_controller.cpp -o controller
elif [ $1 == "lm_data" ]
then
    echo "Compiling lm_data..."
    javac lm_data.java
else
    echo "Compiling arm_updater..."
    g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include -lmysqlcppconn arm_updater.cpp -o updater
    echo "Compiling arm_controller..."
    g++ -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include -lmysqlcppconn arm_controller.cpp -o controller
    echo "Compiling lm_data..."
    javac lm_data.java

fi
