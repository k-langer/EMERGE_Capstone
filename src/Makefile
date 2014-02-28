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

install: install-sql install-qt
install-sql: 
	sudo apt-get install libmysqlcppconn-dev
	sudo apt-get install mysql-client libmysqlclient-dev
install-qt:
	sudo apt-get install qt5-default qttools5-dev-tools
	sudo apt-get build-dep qtmultimedia5-dev
	sudo apt-get install  qtmultimedia5-dev	
	sudo apt-get install libqt53d5
	sudo apt-get install qt3d5-dev
	sudo apt-get install libqt5core5 libQt5Gui5 libQt5Network5 libqt5xml5 libqt5xmlpatterns5 libqt5svg5 libqt5printsupport5 libqt5webkit5

lm_data : $(LM_DATA).java
	$(JCC) -classpath $(JCPATH) -d $(BINARY) $(LM_DATA).java

arm_controller : $(ARM_CONTROLLER).o
	$(CC) -o $(ARM_CONTROLLER).o -c $(SQLFLAGS) $(ARM_CONTROLLER).cpp
	$(CC) -o $(BINARY)Controller $(ARM_CONTROLLER).o $(CPPFLAGS) $(SQLLIBS)

arm_updater : $(ARM_UPDATER).cpp
	$(CC) -o $(ARM_UPDATER).o -c $(SQLFLAGS) $(ARM_UPDATER).cpp
	$(CC) -o $(BINARY)Updater $(ARM_UPDATER).o $(CPPFLAGS) $(SQLLIBS)

clean: 
	rm -f *.class *.o *.log
	rm -rf log/
	rm -rf ../binary/updater ../binary/controller ../binary/*.class
