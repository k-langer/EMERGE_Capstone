#!/bin/bash
lm="LM_DATA"
updater="Updater"
controller="Controller"

lm_pid=`ps aux | grep $lm | grep -v grep | awk '{print $2}'`
if [ -z "$lm_pid" ]
then
    echo "$lm is not found..."
else
    echo "killing $lm"
    kill $lm_pid
    sleep 3
fi
count=`ps aux | grep $lm | grep -v grep | wc -l`
if [ $count -ne 0 ]
then
    echo "[---Failed---] $lm is NOT killed...PID = $lm_pid"
fi

updater_pid=`ps aux | grep $updater | grep -v grep | awk '{print $2}'`
if [ -z "$updater_pid" ]
then
    echo "$updater is not found..."
else
    echo "killing $updater"
    kill $updater_pid
    sleep 3
fi
count1=`ps aux | grep $updater | grep -v grep | wc -l`
if [ $count1 -ne 0 ]
then
    echo "[---Failed---] $updater is NOT killed...PID = $updater_pid"
fi

controller_pid=`ps aux | grep $controller | grep -v grep | awk '{print $2}'`
if [ -z "$controller_pid" ]
then
    echo "$controller is not found..."
else
    echo "killing $controller"
    kill $controller_pid
    sleep 3
fi
count2=`ps aux | grep $controller | grep -v grep | wc -l`
if [ $count2 -ne 0 ]
then
    echo "[---Failed---] $controller is NOT killed...PID = $controller_pid"
fi
