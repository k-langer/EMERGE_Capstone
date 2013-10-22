#!/bin/bash

updater_process="Updater"
updater_name="Updater"
controller_process="Controller"
controller_name="Controller"
lm_process="LM_DATA"
lm_name="Leap Motion Data Gathering"

if [ ! -d "log" ]
then
    mkdir log
fi


updater_count=`ps aux | grep $updater_process | grep -v grep | wc -l`
if [ $updater_count -eq 0 ]
then
    echo "starting $updater_name..."
    ./$updater_process  > log/updater.log &
    sleep 3
else
    echo "The $updater_name has been already running..."
fi
updater_pid=`ps aux | grep $updater_process | grep -v grep | awk '{print $2}'`

if [ -z "$updater_pid" ]
then
    echo "[--Fail--] The $upater_name does not start up..."
else
    echo "The $updater_name is running on pid = $updater_pid "
fi

controller_count=`ps aux | grep $controller_process | grep -v grep | wc -l`
if [ $controller_count -eq 0 ]
then
    echo "starting $controller_name..."
    ./$controller_process > log/controller.log &
    sleep 3
else
    echo "the $contoller_name has already been running..."
fi

controller_pid=`ps aux | grep $controller_process | grep -v grep | awk '{print $2}'`
if [ -z "$controller_pid" ]
then
    echo "[--Fail--] The $controller_name does not start up..."
else
    echo "The $contoller_name is running on pid = $controller_pid"
fi

lm_count=`ps aux | grep $lm_process | grep -v grep | wc -l`
if [ $lm_count -eq 0 ]
then
    echo "starting $lm_name..."
    java -Djava.library.path=/opt/local/LeapSDK/lib $lm_process > log/lm_data.log & 
    sleep 3
else
    echo "the $lm_name has already been running..."
fi

lm_pid=`ps aux | grep $lm_process | grep -v grep | awk '{print $2}'`
if [ -z "$lm_pid" ]
then
    echo "[--Fail--] The $lm_name does not start up..."
else
    echo "The $lm_name is running on pid = $lm_pid"
fi
