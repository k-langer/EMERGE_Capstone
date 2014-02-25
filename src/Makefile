# Makefile: EMERGE

CC = g++
JCC = javac 

SQLFLAGS:=$(shell mysql_config --cflags)
SQLLIBS:=$(shell mysql_config --libs)
CFLAGS = -g -O0 -lm
CPPFLAGS = -lmysqlcppconn
INC = -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include 
LEAPPATH = ../leap/lib/LeapJava.jar
BINARY = ../binary/
JCPATH = "$(LEAPPATH):/opt/local/mysql-connector-java/mysql-connector-java.jar"
ARM_UPDATER = arm_updater
ARM_CONTROLLER = arm_controller

LM_DATA = lm_data

all: $(LM_DATA) $(ARM_UPDATER) $(ARM_CONTROLLER)

install: 
	sudo apt-get install libmysqlcppconn-dev
	sudo apt-get install mysql-client libmysqlclient-dev
	
lm_data : $(LM_DATA).java
	$(JCC) -classpath $(JCPATH) -d $(BINARY) $(LM_DATA).java

arm_controller : $(ARM_CONTROLLER).o
	$(CC) -o $(ARM_CONTROLLER).o -c $(SQLFLAGS) $(ARM_CONTROLLER).cpp
	$(CC) -o $(BINARY)controller $(ARM_CONTROLLER).o $(CPPFLAGS) $(SQLLIBS)

arm_updater : $(ARM_UPDATER).cpp
	$(CC) -o $(ARM_UPDATER).o -c $(SQLFLAGS) $(ARM_UPDATER).cpp
	$(CC) -o $(BINARY)updater $(ARM_UPDATER).o $(CPPFLAGS) $(SQLLIBS)

clean: 
	rm -f *.class *.o *.log
	rm -rf log/
	rm -rf ../binary/updater ../binary/controller ../binary/*.class
