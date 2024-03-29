/******************************************************************************
* Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/
/*
   to compile
    javac -classpath "/opt/local/LeapSDK/lib/LeapJava.jar:/opt/local/mysql-connector-java/mysql-connector-java.jar" lm_data.java   

   to run this program

   java -classpath ".:../lib/LeapJava.jar:/opt/local/mysql-connector-java/mysql-connector-java.jar" -Djava.library.path=../lib LM_DATA 

 */

import java.io.IOException;
import java.lang.Math;
import com.leapmotion.leap.*;
import com.leapmotion.leap.Gesture.State;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Timestamp;
import java.lang.String;
import java.util.Date;

class LM_Listener extends Listener {
    private Hand previous_hand = null;
    private long previous_time;
    private double previous_wristRot;
    private Vector previous_position;
    private double previous_fingers_distance;
    private Connection conn = null;
    private Statement statement = null;
    private int total_frame;
    private int motion;
    private int no_motion;
    public void onInit(Controller controller) {
        System.out.println("Initialized");
        motion = no_motion = total_frame = 0;
        previous_time = 0;
        previous_fingers_distance = -1;
        //mysql connection
        String db_user = "zhen";
        String db_pass = "Zhenjiang#1";
        String db_name = "capstone";
        String db_url = "jdbc:mysql://localhost:3306/"+db_name+"";
        previous_position = null;
        previous_wristRot = -99999;
        //load jdbc driver for mysql
        try {
            System.out.println("Loading driver...");
            Class.forName("com.mysql.jdbc.Driver");
            System.out.println("Driver loaded!");
        } catch (ClassNotFoundException e) {
            throw new RuntimeException("Cannot find the driver in the classpath!", e);
        }
        //get mysql connection
        try {
            conn = DriverManager.getConnection(db_url, db_user, db_pass);
        } catch (SQLException e) {
            // handle any errors
            System.out.println("SQLException: " + e.getMessage());
            System.out.println("SQLState: " + e.getSQLState());
            System.out.println("VendorError: " + e.getErrorCode());
            throw new RuntimeException("", e);
        }
        try {
            statement = conn.createStatement();
        } catch (SQLException e) {
            System.out.println("Unable to create statement");
            throw new RuntimeException("", e);
        }
    }

    public void onConnect(Controller controller) {
        System.out.println("Connected");
    }

    public void onDisconnect(Controller controller) {
        //Note: not dispatched when running in a debugger.
        System.out.println("Disconnected");
    }

    public void onExit(Controller controller) {
        System.out.println("Exited");
    }

    public boolean didItMove(Hand hand1, Hand hand2){
        float distance = hand1.palmPosition().distanceTo(hand2.palmPosition());
        if(distance > 5) return true;
        else return false;
    }
    public static double round(double value, int places) {
        if (places < 0) throw new IllegalArgumentException();
        long factor = (long) Math.pow(10, places);
        value = value * factor;
        long tmp = Math.round(value);
        return tmp / (factor + 0.0);
    }
    public String get_current_time(){
        Date d = new Date();
        Timestamp t = new Timestamp(d.getTime());
        return "[" + t.toString() + "] ";
    }
    public void logger(String msg) {
    	System.out.println(get_current_time() + msg); 
    }
    public void logger(String msg, double value){
        System.out.println(get_current_time() + msg + value);
    }
    public void logger(String msg, float value){
        System.out.println(get_current_time() + msg + value);
    }
    public void logger(String msg1, double value1, String msg2, double value2){
        System.out.println(get_current_time() + msg1 + value1 + " " + msg2 + value2);
    }
    public void logger(String msg1, float value1, String msg2, float value2){
        System.out.println(get_current_time() + msg1 + value1 + " " + msg2 + value2);
    }
    public void logger(String msg1, double value1, String msg2, double value2, String msg3, double value3){
        System.out.println(get_current_time() + msg1 + value1 + " " + msg2 + value2 + " " + msg3 + value3);
    }
    public void logger(String msg1, float value1, String msg2, float value2, String msg3, float value3){
        System.out.println(get_current_time() + msg1 + value1 + " " + msg2 + value2 + " " + msg3 + value3);
    }
    public void logger(String msg, int value){
        System.out.println(get_current_time() + msg + value);
    }
    public void pauseSequence(Frame frame) {
        int finger_num = 0;
        for (Hand hand : frame.hands()) {
            finger_num+=hand.fingers().count(); 
        }        
        if (finger_num == 10) {
            try {
                Thread.sleep(1000);
            } catch(InterruptedException ex) {
                Thread.currentThread().interrupt();
            }
        }
        while (finger_num > 6) {
            try {
                Thread.sleep(100); 
            } catch(InterruptedException ex) {
                 Thread.currentThread().interrupt();
            }
            finger_num = 0; 
            for (Hand hand : frame.hands()) {
                finger_num+=hand.fingers().count(); 
            }
        }
    }
    double xpos = 0;
    double ypos = 0; 
    double zpos = 0; 
    double wrot = 0; 
    double wang = 0;   
    public void onFrame(Controller controller) {
        // Get the most recent frame and report some basic information
        Frame frame = controller.frame();
        String msg;
        //pause the listener
        if(frame.hands().count() > 1){
            try{
                Thread.sleep(3000);//sleep for three seconds
            }catch(InterruptedException ex) {
                Thread.currentThread().interrupt();
            }
            return;
        } 
        if (frame.hands().count() > 0 ) {
            // Get the first hand
            Hand hand = frame.hands().get(0);

            //time
            long current_time = frame.timestamp();
            long dTime = 0;
            if(previous_time == 0) dTime = 1000;
            else dTime = (current_time - previous_time)/1000;
           
            if(dTime < 150) return;
            previous_time = current_time;
            total_frame++;

            Vector v = hand.palmVelocity();
            double speed = Math.sqrt(Math.pow(v.getX(), 2)
                           + Math.pow(v.getZ(), 2)
                           + Math.pow(v.getY(), 2));
            if(speed > 400){
                logger("too fast......speed: " + String.valueOf((long)speed));
                return;
            }
            //wrist rot
            double wristRot = Math.toDegrees(hand.palmNormal().roll());

            //xyz position = palm_position
            Vector pos = hand.palmPosition();

            double MOTION_DISTANCE = 12.1;
            double WRISTROT_CHANGE = 5;
            double FINGERS_DISTANCE_CHANGE = 5;

            //two fingers distance
            FingerList fingers = hand.fingers();
            double left_size = 3;
            double right_size = 3;
            double f_distance = -1;
            if (fingers.count() > 0) {
                Finger left_finger = frame.fingers().leftmost();
                Finger right_finger = frame.fingers().rightmost();
                f_distance = left_finger.tipPosition().distanceTo(
                             right_finger.tipPosition());
                if(f_distance != 0) 
                    f_distance = f_distance - right_size - left_size;
            }else{
                f_distance = previous_fingers_distance;
            }
            f_distance /= 5;
            if(previous_position != null && wristRot != -99999 
               && previous_fingers_distance != -1){
               double dPos = Math.abs(pos.distanceTo(previous_position)); 
               double dWrot = Math.abs(wristRot - previous_wristRot);
               double dFin = Math.abs(f_distance - previous_fingers_distance);
               if(dFin > 30){
                    logger("Fingers are changing to fast... set to 0");
                    logger("dFin:", dFin);
                    dFin = 0;
               }
               if(dPos < MOTION_DISTANCE && 
                  dWrot < WRISTROT_CHANGE && 
                  dFin < FINGERS_DISTANCE_CHANGE){
                   no_motion++;
                   return;
               } else if(dPos > MOTION_DISTANCE &&
                       dWrot > WRISTROT_CHANGE &&
                       dFin > FINGERS_DISTANCE_CHANGE){
                   logger("Motion Dectected...All of three");
                   motion++;
                   previous_position = pos;
                   previous_wristRot = wristRot;
                   previous_fingers_distance = f_distance;
               
               } else if(dPos > MOTION_DISTANCE && dWrot > WRISTROT_CHANGE){
                   logger("Motion Dectected...Position + Wrot");
                   motion++;
                   logger("dPos: ", dPos, "Min: ", MOTION_DISTANCE);
                   logger("dWrot: ", dWrot, " Min: ", WRISTROT_CHANGE);
                   previous_position = pos;
                   previous_wristRot = wristRot;
                   //f_distance = previous_fingers_distance;
               } else if(dPos > MOTION_DISTANCE && dFin > FINGERS_DISTANCE_CHANGE){
                   logger("Motion Dectected... Position + Finger");
                   motion++;
                   logger("dPos: ", dPos, "Min: ", MOTION_DISTANCE);
                   logger("dFin: ", dFin, " Min: ", FINGERS_DISTANCE_CHANGE);
                   previous_position = pos;
                   previous_fingers_distance = f_distance;
                   //wristRot = previous_wristRot;
               } else if(dWrot > WRISTROT_CHANGE && dFin > FINGERS_DISTANCE_CHANGE){
                   logger("Motion Dectected...Wrot + Finger");
                   motion++;
                   logger("dFin: ", dFin, " Min: ", FINGERS_DISTANCE_CHANGE);
                   logger("dWrot: ", dWrot, " Min: ", WRISTROT_CHANGE);
                   previous_fingers_distance = f_distance;
                   previous_wristRot = wristRot;
                   pos = previous_position;
               } else if(dPos > MOTION_DISTANCE){
                   logger("Motion Dectected...Position");
                   motion++;
                   logger("dPos: ", dPos, "Min: ", MOTION_DISTANCE);
                   previous_position = pos;
                   //wristRot = previous_wristRot;
                   //f_distance = previous_fingers_distance;
               } else if(dWrot > WRISTROT_CHANGE){
                   logger("Motion Dectected...Wrot");
                   motion++;
                   logger("dWrot: ", dWrot, " Min: ", WRISTROT_CHANGE);
                   previous_wristRot = wristRot;
                   //f_distance = previous_fingers_distance;
                   pos = previous_position;
               } else if(dFin > FINGERS_DISTANCE_CHANGE){
                   logger("Motion Dectected...Finger");
                   motion++;
                   logger("dFin: ", dFin, " Min: ", FINGERS_DISTANCE_CHANGE);
                   previous_fingers_distance = f_distance;
                   pos = previous_position;
                   wristRot = previous_wristRot;
               }
            }
            previous_position = pos;
            previous_fingers_distance = f_distance;
            previous_wristRot = wristRot;

            //wrist angle
            double wristAngle = Math.toDegrees(hand.direction().pitch());

            //compose input string
            int precision = 5;
	    xpos = 0 - pos.getX(); 
	    ypos = pos.getY(); 
	    zpos = 0 - pos.getZ(); 
	    wang = wristAngle; 
	    wrot = wristRot; 
	
	    String result = "03 " + round(zpos, precision) + " " + 
                            round(xpos, precision) + " " + 
                            round(ypos, precision) + " " + 
                            round(wang, precision) + " " + 
                            round(wrot, precision) + " " + 
                            dTime + " 1 " + round(f_distance, 5);
            logger("Insert data: " + result);
            String query = "insert into xyz values(Default,'" + result + "');";
            exec_query(this.statement, query);
            result = "" + round(zpos, precision) + " " +
                            round(xpos, precision) + " " + 
                            round(ypos, precision);
            query = "insert into xyz_tmp values(Default,'" + result + "');";
            exec_query(this.statement, query);
        }

    }
    public void exec_query(Statement statement, String query){
        try{
            statement.executeUpdate(query);
        }catch(SQLException ex){
            System.out.println(ex.getMessage());
        }
    } 
    private double alpha = 0.2961;
    public double exp_avg(double value, double oldValue) {
	double newValue = oldValue + alpha * (value - oldValue);
	oldValue = newValue; 
	return newValue; 
    }
}

class LM_DATA {
    public static void main(String[] args) {
               // Create a sample listener and controller
        LM_Listener listener = new LM_Listener();
        Controller controller = new Controller();

        // Have the sample listener receive events from the controller
        controller.addListener(listener);
        while(true){}
    }
}
