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

if [ $1 == "mysqld" ]
then
    echo "Starting mysqld..."
    `start_mysql`
fi
