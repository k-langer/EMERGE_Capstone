/******************************************************************************\
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

   java -classpath ".:../lib/LeapJava.jar:/opt/local/mysql-connector-java/mysql-connector-java.jar" -Djava.library.path=../lib Sample

 */

import java.io.IOException;
import java.lang.Math;
import com.leapmotion.leap.*;
import com.leapmotion.leap.Gesture.State;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.lang.String;

class SampleListener extends Listener {

    private long previous_time;
    private Connection conn = null;
    private Statement statement = null;
    public void onInit(Controller controller) {
        System.out.println("Initialized");
        previous_time = 0;
        //mysql connection
        String db_user = "zhen";
        String db_pass = "Zhenjiang#1";
        String db_name = "capstone";
        String db_url = "jdbc:mysql://localhost:3306/"+db_name+"";
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
        controller.enableGesture(Gesture.Type.TYPE_SWIPE);
        controller.enableGesture(Gesture.Type.TYPE_CIRCLE);
        controller.enableGesture(Gesture.Type.TYPE_SCREEN_TAP);
        controller.enableGesture(Gesture.Type.TYPE_KEY_TAP);
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
        return (double) tmp / factor;
    }
    public void onFrame(Controller controller) {
        // Get the most recent frame and report some basic information
        Frame frame = controller.frame();

        if (!frame.hands().empty()) {
            // Get the first hand
            Hand hand = frame.hands().get(0);

            //time
            long current_time = frame.timestamp();
            long dTime = 0;
            if(previous_time == 0) dTime = 1000;
            else dTime = (current_time - previous_time)/1000;
           
            if(dTime < 1000) return;
            System.out.println(frame.hands().get(0).fingers().count()); 
            Vector v = hand.palmVelocity();
            double speed = Math.sqrt(Math.pow(v.getX(),2)+Math.pow(v.getZ(),2)+Math.pow(v.getY(),2)); 
            System.out.println("Palm Speed:"+speed);
            if(speed > 400){
                System.out.println("too fast......speed: " + speed);
                return;
            }

            //xyz position = palm_position
            Vector pos = hand.palmPosition();

            //wrist angle
            double wristAngle = Math.toDegrees(hand.direction().pitch());

            //wrist rot
            double wristRot = Math.toDegrees(hand.palmNormal().roll());

            //two fingers distance
            FingerList fingers = hand.fingers();
            double left_size = 3;
            double right_size = 3;
            double f_distance = 100;
            if (!fingers.empty()) {
                Finger left_finger = frame.fingers().leftmost();
                Finger right_finger = frame.fingers().rightmost();
                f_distance = left_finger.tipPosition().distanceTo(right_finger.tipPosition());
                if(f_distance != 0) 
                    f_distance = f_distance - right_size - left_size;
            }
            
            //compose input string
            int precision = 5;
            String result = "03 " + round(0 - pos.getZ(), precision) + " " + 
                            round(0 - pos.getX(), precision) + " " + 
                            round(pos.getY(), precision) + " " + 
                            round(wristAngle, precision) + " " + 
                            round(wristRot, precision) + " " + 
                            dTime + " 1 " + round(f_distance, 5);
            System.out.println(result);
            previous_time = current_time;
            String query = "insert into xyz values(Default,'" + result + "');";
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
}

class Sample {
    public static void main(String[] args) {
               // Create a sample listener and controller
        SampleListener listener = new SampleListener();
        Controller controller = new Controller();

        // Have the sample listener receive events from the controller
        controller.addListener(listener);
        while(true){}

    }
}
