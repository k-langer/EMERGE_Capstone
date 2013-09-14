# Makefile: EMERGE

CC = g++
JCC = javac 

CFLAGS = -g -O0 -lm
CPPFLAGS = -lmysqlcppconn
INC = -I/opt/local/include -I/usr/local/include/cppconn -I/usr/local/include 
LEAPPATH = /opt/local/LeapSDK/lib/LeapJava.jar

JCPATH = "$(LEAPPATH):/opt/local/mysql-connector-java/mysql-connector-java.jar"
ARM_UPDATER = arm_updater
ARM_CONTROLLER = arm_controller 

LM_DATA = lm_data

all: lm_data

lm_data : $(LM_DATA).java
	$(JCC) -classpath $(JCPATH) $(LM_DATA).java
arm_updater : $(ARM_UPDATER).cpp
	$(CC) $(INC) $(CPPFLAGS) $(ARM_UPDATER).cpp -o $(ARM_UPDATER)
arm_controller : $(ARM_CONTROLLER).cpp 
	$(CC) $(INC) $(CPPFLAGS) $(ARM_CONTROLLER).cpp -o ($AMR_CONTROLLER)
clean: 
	rm -f *.class *.o *.log
