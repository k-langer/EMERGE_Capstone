
//=============================================================================
//Project Kurt's PhantomX Reactor Arm

// NEEDS DESCRIPTION


//  This code is a Work In Progress and is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//  
//=============================================================================

//=============================================================================
// Define Options
//=============================================================================

#define OPT_WRISTROT           // comment this out if you are not using the wrist rotate
#define ARBOTIX_TO  1000      // if no message for a second probably turned off...
#define SOUND_PIN    1      // Tell system we have added speaker to IO pin 1
#define MAX_SERVO_DELTA_PERSEC 255
//#define DEBUG             // Enable Debug mode via serial


//=============================================================================
// Global Include files
//=============================================================================
#include <ax12.h>
#include <BioloidController.h>
#include <ArmControl.h>
#include <Wire.h> 
#include <math.h>



//=============================================================================
//=============================================================================
/* Servo IDs */
enum {
  SID_BASE=1, SID_RSHOULDER, SID_LSHOULDER, SID_RELBOW, SID_LELBOW, SID_WRIST, SID_WRISTROT, SID_GRIP};
enum {
  IKM_IK3D_CARTESIAN, IKM_CYLINDRICAL};
enum {
  IKS_SUCCESS=0, IKS_WARNING, IKS_ERROR};


#define CNT_SERVOS  8

#define IK_MAX_X  300
#define IK_MIN_X  -300

#define IK_MAX_Y  350
#define IK_MIN_Y  50

#define IK_MAX_Z  350
#define IK_MIN_Z  20

#define IK_MAX_GA  90
#define IK_MIN_GA   -90

// Define Ranges for the different servos...
#define BASE_MIN    0
#define BASE_MAX    1023

#define SHOULDER_MIN  205 
#define SHOULDER_MAX  810

#define ELBOW_MIN    210
#define ELBOW_MAX    900

#define WRIST_MIN    200
#define WRIST_MAX    830

#define WROT_MIN     0
#define WROT_MAX     1023

#define GRIP_MIN     0
#define GRIP_MAX     512

// Define some lengths and offsets used by the arm
#define BaseHeight          110L   // (L0)about 120mm 
#define ShoulderLength      150L   // (L1)Not sure yet what to do as the servo is not directly in line,  Probably best to offset the angle?
//                                 // X is about 140, y is about 40 so sqrt is Hyp is about 155, so maybe about 21 degrees offset
#define ShoulderServoOffset 72L    // should offset us some...
#define ElbowLength         147L   //(L2)Length of the Arm from Elbow Joint to Wrist Joint
#define WristLength         137L   // (L3)Wrist length including Wrist rotate
#define G_OFFSET            0      // Offset for static side of gripper?

#define IK_FUDGE            5     // How much a fudge between warning and error

//=============================================================================
// Global Objects
//=============================================================================
ArmControl armcontrol = ArmControl();
BioloidController bioloid = BioloidController(1000000);
//LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//=============================================================================
// Global Variables...
//=============================================================================
boolean         g_fArmActive = false;   // Is the arm logically on?
byte            g_bIKMode = IKM_IK3D_CARTESIAN;   // Which mode of operation are we in...
uint8_t         g_bIKStatus = IKS_SUCCESS;   // Status of last call to DoArmIK;
boolean         g_fServosFree = true;

// Current IK values
int            g_sIKGA;                  // IK Gripper angle..
int            g_sIKX;                  // Current X value in mm
int            g_sIKY;                  //
int            g_sIKZ;

// Values for current servo values for the different joints
int             g_sBase;                // Current Base servo value
int             g_sShoulder;            // Current shoulder target 
int             g_sElbow;               // Current
int             g_sWrist;               // Current Wrist value
int             g_sWristRot;            // Current Wrist rotation
int             g_sGrip = 256;                // Current Grip position

// BUGBUG:: I hate too many globals...
int sBase, sShoulder, sElbow, sWrist, sWristRot, sGrip;
int sDeltaTime = 900;


// Message informatino
unsigned long   ulLastMsgTime;          // Keep track of when the last message arrived to see if controller off
byte            buttonsPrev;            // will use when we wish to only process a button press once


//
#ifdef DEBUG
boolean        g_fDebugOutput = false;
#endif

// Forward references
extern void MSound(byte cNotes, ...);


//===================================================================================================
// Setup 
//====================================================================================================
void setup() {
  pinMode(0,OUTPUT);  
  
  Serial.begin(38400);
  Serial.println("Reactor Online.");
  
  // Next initialize the Bioloid
  bioloid.poseSize = CNT_SERVOS;

  // Read in the current positions...
  bioloid.readPose();
  
  // Start off to put arm to home position...
  MoveArmTo(512, 210, 215, 710, 512, 256, sDeltaTime, true);

  MSound(3, 60, 2000, 80, 2250, 900, 2500);
  Serial.println("MSound execute");
  Serial.println("Move to home");

  //set Gripper Compliance so it doesn't tear itself apart
  ax12SetRegister(SID_GRIP, AX_CW_COMPLIANCE_SLOPE, 128);
  ax12SetRegister(SID_GRIP, AX_CCW_COMPLIANCE_SLOPE, 128);
  Serial.println("Setup Complete");

}


//===================================================================================================
// loop: Our main Loop!
//===================================================================================================
int getInputValue(int numByte, boolean firstSignByte = false, char* msg = "" ){
    //Serial.println(msg);
    int a[numByte];
    int result = 0;
    int trueNumByte = numByte;
    if(firstSignByte == true){
      trueNumByte -= 1;
    }
    for(int i = 0; i < numByte; i++){
        while(Serial.available() <= 0){}
        a[i] = Serial.read() - 48;
        //Serial.println(a[i]);
        if(i == 0 && firstSignByte == true){//negative value
          //do nothing
        }else{
          int temp = a[i];
          for(int j = 0; j < trueNumByte - 1; j++){
            temp *= 10; 
          }
          trueNumByte--;
          result += temp;
        }
    }
    if(firstSignByte == true && a[0] == 1){
       result = 0 - result; 
    }
    return result;
    
}
void sleep(int t){
  while(bioloid.interpolating > 0){  // do this while we have not reached our new pose
        bioloid.interpolateStep();     // move servos, if necessary. 
        Serial.print("moving");
        delay(t/5);
  }
  
}
//MoveArmTo(int sBase, int sShoulder, int sElbow, int sWrist, int sWristRot, int sGrip, int wTime, boolean fWait)
// MoveArmTo(512, 212, 212, 512, 512, 256, sDeltaTime, true); 
void loop(){
  //01 sleep
  //02 torque on
  //03 perform action
  int action = getInputValue(2, false);
  if(action == 1){
    Serial.println("Sleep");
    getInputValue(23, false);
    PutArmToSleep();
    Serial.println('\t');
  }else if(action == 2){
    Serial.println("Reboot");
    getInputValue(23, false);
    setup();
    Serial.println('\t');
  }else if(action == 3){
    int base, shoulder, elbow, wrist, wristRot, grip, waitTime;
    boolean ifWait;
    base = getInputValue(3, false);
    shoulder = getInputValue(3, false);
    elbow = getInputValue(3, false);
    wrist = getInputValue(3, false);
    wristRot = getInputValue(3, false);
    grip = getInputValue(3, false);
    waitTime = getInputValue(4, false);
    int temp = getInputValue(1, false);
    if(temp == 0){
       ifWait = false; 
    }else{
       ifWait = true; 
    }
    MoveArmTo(base, shoulder, elbow, wrist, wristRot, grip, waitTime, ifWait);
    Serial.println('\t');   
  } 
}

//===================================================================================================
// Determine where the gripper is gripping something
int GRIPPING_VALUE = 100;
boolean isGripping(void){
  int val = analogRead(A0);
  if(val >= GRIPPING_VALUE)
    return true;
  else
    return false;
}
//===================================================================================================
//===================================================================================================
// PutArmToSleep
//===================================================================================================
void PutArmToSleep(void) {
  g_fArmActive = false;
  MoveArmTo(512, 212, 412, 352, 512, 256, sDeltaTime, true);

  // And Relax all of the servos...
  for(uint8_t i=1; i <= CNT_SERVOS; i++) {
    Relax(i);
  }
  g_fServosFree = true;
}

//===================================================================================================
// MoveArmTo
//===================================================================================================
void MoveArmTo(int sBase, int sShoulder, int sElbow, int sWrist, int sWristRot, int sGrip, int wTime, boolean fWait) {

  //prevent gripper change to smaller value if gripper is gripping
  if(isGripping()){
    if(sGrip < g_sGrip){
      sGrip = g_sGrip;
    }
  }
  int sMaxDelta;
  int sDelta;

  // First make sure servos are not free...
  if (g_fServosFree) {
    g_fServosFree = false;
    for(uint8_t i=1; i <= CNT_SERVOS; i++) {
      TorqueOn(i);
    }
    bioloid.readPose();
  }

  // Make sure the previous movement completed.
  // Need to do it before setNextPos calls as this
  // is used in the interpolating code...
  while (bioloid.interpolating > 0) {
    bioloid.interpolateStep();
    delay(3);
  }
  
  g_sGrip = sGrip;

  // Also lets limit how fast the servos will move as to not get whiplash.
  bioloid.setNextPose(SID_BASE, sBase);

  sMaxDelta = abs(bioloid.getCurPose(SID_RSHOULDER) - sShoulder);
  bioloid.setNextPose(SID_RSHOULDER, sShoulder);
  bioloid.setNextPose(SID_LSHOULDER, 1024-sShoulder);

  sDelta = abs(bioloid.getCurPose(SID_RELBOW) - sElbow);
  if (sDelta > sMaxDelta)
    sMaxDelta = sDelta;
  bioloid.setNextPose(SID_RELBOW, sElbow);
  bioloid.setNextPose(SID_LELBOW, 1024-sElbow);

  sDelta = abs(bioloid.getCurPose(SID_WRIST) - sWrist);
  if (sDelta > sMaxDelta)
    sMaxDelta = sDelta;
  bioloid.setNextPose(SID_WRIST, sWrist);

  bioloid.setNextPose(SID_WRISTROT, sWristRot);  

  bioloid.setNextPose(SID_GRIP, sGrip);

  bioloid.interpolateSetup(wTime);

  // Do at least the first movement
  bioloid.interpolateStep();
  // And if asked to, wait for the previous move to complete...
  if (fWait) {
    while (bioloid.interpolating > 0) {
      bioloid.interpolateStep();
      delay(3);
    }
  }
}

//test code for now
void isGripLoaded(){
  //int post = ax12GetRegister(SID_GRIP, AX_PRESENT_POSITION_L, 2);
  int load = ax12GetRegister(SID_GRIP, AX_PRESENT_LOAD_L, 2);
  Serial.print("Grip load:");
  Serial.println(load);
  //Serial.print("Grip pos:");
  //Serial.println(post);
}

//===================================================================================================
// Convert radians to servo position offset. 
//===================================================================================================
int radToServo(float rads){
  float val = (rads*100)/51 * 100;
  return (int) val;
}


//===================================================================================================
// Compute Arm IK for 3DOF+Mirrors+Gripper - was based on code by Michael E. Ferguson
// Hacked up by me, to allow different options...
//===================================================================================================
uint8_t doArmIK(boolean fCartesian, int sIKX, int sIKY, int sIKZ, int sIKGA)
{
  int t;
  int sol0;
  uint8_t bRet = IKS_SUCCESS;  // assume success
#ifdef DEBUG
  if (g_fDebugOutput) {
    Serial.print("(");
    Serial.print(sIKX, DEC);
    Serial.print(",");
    Serial.print(sIKY, DEC);
    Serial.print(",");
    Serial.print(sIKZ, DEC);
    Serial.print(",");
    Serial.print(sIKGA, DEC);
    Serial.print(")=");
  }
#endif
  if (fCartesian) {
    // first, make this a 2DOF problem... by solving base
    sol0 = radToServo(atan2(sIKX,sIKY));
    // remove gripper offset from base
    t = sqrt(sq((long)sIKX)+sq((long)sIKY));
    // BUGBUG... Not sure about G here
#define G 30   
    sol0 -= radToServo(atan2((G/2)-G_OFFSET,t));
  }
  else {
    // We are in cylindrical mode, probably simply set t to the y we passed in...
    t = sIKY;
#ifdef DEBUG
    sol0 = 0;
#endif
  }
  // convert to sIKX/sIKZ plane, remove wrist, prepare to solve other DOF           
  float flGripRad = (float)(sIKGA)*3.14159/180.0;
  long trueX = t - (long)((float)WristLength*cos(flGripRad));   
  long trueZ = sIKZ - BaseHeight - (long)((float)WristLength*sin(flGripRad));

  long im = sqrt(sq(trueX)+sq(trueZ));        // length of imaginary arm
  float q1 = atan2(trueZ,trueX);              // angle between im and X axis
  long d1 = sq(ShoulderLength) - sq(ElbowLength) + sq((long)im);
  long d2 = 2*ShoulderLength*im;
  float q2 = acos((float)d1/float(d2));
  q1 = q1 + q2;
  int sol1 = radToServo(q1-1.57);

  d1 = sq(ShoulderLength)-sq((long)im)+sq(ElbowLength);
  d2 = 2*ElbowLength*ShoulderLength;
  q2 = acos((float)d1/(float)d2);
  int sol2 = radToServo(3.14-q2);

  // solve for wrist rotate
  int sol3 = radToServo(3.2 + flGripRad - q1 - q2 );

#ifdef DEBUG
  if (g_fDebugOutput) {
    Serial.print("<");
    Serial.print(sol0, DEC);
    Serial.print(",");
    Serial.print(trueX, DEC);
    Serial.print(",");
    Serial.print(trueZ, DEC);
    Serial.print(",");
    Serial.print(sol1, DEC);
    Serial.print(",");
    Serial.print(sol2, DEC);
    Serial.print(",");
    Serial.print(sol3, DEC);
    Serial.print(">");
  }
#endif   

  // Lets calculate the actual servo values.

  if (fCartesian) {
    sBase = min(max(512 - sol0, BASE_MIN), BASE_MAX);
  }
  sShoulder = min(max(512 - sol1, SHOULDER_MIN), SHOULDER_MAX);

  // Magic Number 819???
  sElbow = min(max(819 - sol2, SHOULDER_MIN), SHOULDER_MAX);

#define Wrist_Offset 512
  sWrist = min(max(Wrist_Offset + sol3, WRIST_MIN), WRIST_MAX);

  // Remember our current IK positions
  g_sIKX = sIKX; 
  g_sIKY = sIKY;
  g_sIKZ = sIKZ;
  g_sIKGA = sIKGA;
  // Simple test im can not exceed the length of the Shoulder+Elbow joints...

  if (im > (ShoulderLength + ElbowLength)) {
    if (g_bIKStatus != IKS_ERROR) {
#ifdef DEBUG
      if (g_fDebugOutput) {
        Serial.println("IK Error");
      }
#endif
      MSound(2, 50, 3000, 50, 3000);
    }
    bRet = IKS_ERROR;  
  }
  else if(im > (ShoulderLength + ElbowLength-IK_FUDGE)) {
    if (g_bIKStatus != IKS_WARNING) {
#ifdef DEBUG
      if (g_fDebugOutput) {
        Serial.println("IK Warning");
      }
#endif
      MSound(1, 75, 2500);
    }
    bRet = IKS_WARNING;  
  }

  return bRet;
}




// BUGBUG:: Move to some library...
//==============================================================================
//    SoundNoTimer - Quick and dirty tone function to try to output a frequency
//            to a speaker for some simple sounds.
//==============================================================================
#ifdef SOUND_PIN
void SoundNoTimer(unsigned long duration,  unsigned int frequency)
{
#ifdef __AVR__
  volatile uint8_t *pin_port;
  volatile uint8_t pin_mask;
#else
  volatile uint32_t *pin_port;
  volatile uint16_t pin_mask;
#endif
  long toggle_count = 0;
  long lusDelayPerHalfCycle;

  // Set the pinMode as OUTPUT
  pinMode(SOUND_PIN, OUTPUT);

  pin_port = portOutputRegister(digitalPinToPort(SOUND_PIN));
  pin_mask = digitalPinToBitMask(SOUND_PIN);

  toggle_count = 2 * frequency * duration / 1000;
  lusDelayPerHalfCycle = 1000000L/(frequency * 2);

  // if we are using an 8 bit timer, scan through prescalars to find the best fit
  while (toggle_count--) {
    // toggle the pin
    *pin_port ^= pin_mask;

    // delay a half cycle
    delayMicroseconds(lusDelayPerHalfCycle);
  }    
  *pin_port &= ~(pin_mask);  // keep pin low after stop

}

void MSound(byte cNotes, ...)
{
  va_list ap;
  unsigned int uDur;
  unsigned int uFreq;
  va_start(ap, cNotes);

  while (cNotes > 0) {
    uDur = va_arg(ap, unsigned int);
    uFreq = va_arg(ap, unsigned int);
    SoundNoTimer(uDur, uFreq);
    cNotes--;
  }
  va_end(ap);
}
#else
void MSound(byte cNotes, ...)
{
};
#endif
