/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]         
// Controller1          controller           
// rightMotor           motor         1           
// leftMotor            motor         2               
// liftMotor            motor         3
// lowArmMotor          motor         4        
// lowVision            vision        20                  
// highVision           vision        19           
// ---- END VEXCODE CONFIGURED DEVICES ----

// |-------------------- Library Imports --------------------|

#include <stdio.h>
#include "vex.h"
using namespace vex;

// |-------------------- Initialize Global Variables --------------------|

int highCenterX = 158;          // half the number of pixels of the vision sensor
int highBottomY = 175;          // bottom of vision sensor field of view
int lowBottomY = 190;

double speedMultiplierX = 0.35;  // multiplier used when calculating speed based on distance from object on x-axis
double speedMultiplierY = 0.75;  // multiplier used when calculating speed based on distance from object on y-axis

double liftAngle = 70;
int liftSpeed = 50;

bool canDrive = true;

int focusobj;

task driveMotors;
task calibrateliftMotor;
task centerTOgoal;

// |-------------------- Class Definitions --------------------|

class driveTrain
{
  public:
  
  driveTrain()
  {
    canDrive = true;

    refreshMotors();

    Controller1.Axis3.changed(setDirectionVelocity);
    Controller1.Axis4.changed(setDirectionVelocity);
  }

  static void setDirectionVelocity()
  {
    int updown = Controller1.Axis3.position() ^ 3 / 20000;
    int leftright = Controller1.Axis4.position() ^ 3 / 20000;

    int leftVelocity = updown, rightVelocity = updown;

    if (updown == 0)
    {
      if (leftright > 0)
      {
        leftVelocity += abs(leftright);
        rightVelocity -= abs(leftright);
      }
      else if (leftright < 0)
      {
        rightVelocity += abs(leftright);
        leftVelocity -= abs(leftright);
      }
    }
    else if (updown > 0)
    {
      if (leftright > 0)
      {
        leftVelocity += abs(leftright);
      }
      else if (leftright < 0)
      {
        rightVelocity += abs(leftright);
      }
    }
    else if (updown < 0)
    {
      if (leftright > 0)
      {
        leftVelocity -= abs(leftright);
      }
      else if (leftright < 0)
      {
        rightVelocity -= abs(leftright);
      }
    }

    leftMotor.setVelocity(leftVelocity, velocityUnits::pct);
    rightMotor.setVelocity(rightVelocity, velocityUnits::pct);
  }

  void refreshMotors()
  {
    leftMotor.setVelocity(0, velocityUnits::pct);
    rightMotor.setVelocity(0, velocityUnits::pct);
  }
};

class lift
{
  public:

  lift()
  {
    calibrateliftMotor = task(liftCalibrate, vex::task::taskPriorityNormal);
    calibrateliftMotor.suspend();

    Controller1.ButtonUp.pressed(liftUp);
    Controller1.ButtonDown.pressed(liftDown);
    Controller1.ButtonRight.pressed(resetLiftMotorEncoder);
    Controller1.ButtonLeft.pressed(recalibrateStart);
    Controller1.ButtonLeft.released(recalibrateStop);
  }

  static void liftUp()
  {
    recalibrateStop();
    liftMotor.spinTo(liftAngle, rotationUnits::deg, liftSpeed, velocityUnits::pct, true);
  }

  static void liftDown()
  {
    recalibrateStop();
    liftMotor.spinTo(0, rotationUnits::deg, liftSpeed, velocityUnits::pct, true);
  }

  static void resetLiftMotorEncoder()
  {
    recalibrateStop();
    liftMotor.resetRotation();
  }

  static void recalibrateStart()
  {
    liftMotor.setVelocity(10, velocityUnits::pct);
    calibrateliftMotor.resume();
  }

  static void recalibrateStop()
  {
    calibrateliftMotor.suspend();
  }

  static int liftCalibrate()
  {
    while (true)
    {
      liftMotor.spin(forward);

      wait(25, msec);
    }
    return 0;
  }
};

class centerAssistTool
{
  public:

  centerAssistTool()
  {
    Controller1.ButtonY.pressed(centerStart);
    Controller1.ButtonY.released(centerStop);

    centerTOgoal = task(focus, vex::task::taskPriorityHigh);
    centerTOgoal.suspend();
  }

  static void centerStart()
  {
    driveMotors.suspend();
    centerTOgoal.resume();
  }

  static void centerStop()
  {
    driveMotors.resume();
    centerTOgoal.suspend();
  }

  static void snap(int sig, int* lowObjects, int* highObjects)
  {
    // take snapshot with vision camera and return number of signature objects in frame
    switch (sig)
    {
      case 0:
        *lowObjects = lowVision.takeSnapshot(LOWREDGOAL);
        *highObjects = highVision.takeSnapshot(HIGHREDGOAL);
        break;
      case 1:
        *lowObjects = lowVision.takeSnapshot(LOWBLUEGOAL);
        *highObjects = highVision.takeSnapshot(HIGHBLUEGOAL);
        break;
      case 2:
        *lowObjects = lowVision.takeSnapshot(LOWYELLOWGOAL);
        *highObjects = highVision.takeSnapshot(HIGHYELLOWGOAL);
        break;
    }
  }

  // find(object): function that returns a boolean of whether the object is in vision
  static void scan(int sig, bool* lowState, bool* highState) // sig parameter is the object to detect
  {
    int lowObjects;
    int highObjects;

    snap(sig, &lowObjects, &highObjects);

    if (lowObjects != 0) *lowState = true;
    else *lowState = false;
    if (highObjects != 0) *highState = true;
    else *highState = false;
  }

  static void chooseVisionSensor(int sig, int* x)
  {
    bool highState, lowState;

    int highX, highY, lowX, lowY, lowWidth;

    while (true)
    {
      scan(sig, &lowState, &highState);

      highX = highVision.largestObject.centerX;
      highY = highVision.largestObject.centerY;
      lowX = lowVision.largestObject.centerX;
      lowY = lowVision.largestObject.centerY;
      lowWidth = lowVision.largestObject.width;

      if (!highState && !lowState)
      {
        Controller1.Screen.clearLine();
        Controller1.Screen.print("no object");
        return;
      }
      else if (highState && highY < highBottomY)
      {
        Controller1.Screen.clearLine();
        Controller1.Screen.print("high cam");
        *x = highX;
        break;
      }
      else if (lowState && lowY < lowBottomY)
      {
        Controller1.Screen.clearLine();
        Controller1.Screen.print("low cam");
        *x = lowX;
        break;
      }
      else
      {
        Controller1.Screen.clearLine();
        Controller1.Screen.print("BACKUP");

        leftMotor.setVelocity(-50, velocityUnits::pct);
        rightMotor.setVelocity(-50, velocityUnits::pct);

        while(lowY > lowBottomY)
        {
          scan(sig, &lowState, &highState);
          lowY = lowVision.largestObject.centerY;

          leftMotor.spin(forward);
          rightMotor.spin(forward);
        }
        leftMotor.stop();
        rightMotor.stop();
      }
      wait(50, msec);
    }
  }

  static void findFocusObj(int* sig)
  {
    int redArea;
    int blueArea;
    int yellowArea;

    lowVision.takeSnapshot(LOWREDGOAL);
    highVision.takeSnapshot(HIGHREDGOAL);
    redArea = ((lowVision.largestObject.width*lowVision.largestObject.height)+(highVision.largestObject.width*highVision.largestObject.height))/2;

    lowVision.takeSnapshot(LOWBLUEGOAL);
    highVision.takeSnapshot(HIGHBLUEGOAL);
    blueArea = ((lowVision.largestObject.width*lowVision.largestObject.height)+(highVision.largestObject.width*highVision.largestObject.height))/2;

    lowVision.takeSnapshot(LOWYELLOWGOAL);
    highVision.takeSnapshot(HIGHYELLOWGOAL);
    yellowArea = ((lowVision.largestObject.width*lowVision.largestObject.height)+(highVision.largestObject.width*highVision.largestObject.height))/2;

    *sig = 0;
    if (blueArea > redArea)
    {
      *sig = 1;
      if (yellowArea > blueArea)
      {
        *sig = 2;
      }
    }
    else if (yellowArea > redArea)
    {
      *sig = 2;
    }
  }

  // focus(object): function that rotates the robot until the object in vision is centered on the x-axis
  static int focus()
  {
    // initialize variables
    int x;

    double speed = 0;
    bool linedUp = false;

    // while loop until robot is lined up with object in vision on the x-axis
    while (!linedUp)
    {
      // take snapshot with vision camera and return number of signature objects in frame

      findFocusObj(&focusobj);

      chooseVisionSensor(focusobj, &x);

      // if the object's x-coordinate is to the left of the center of vision
      if (x > highCenterX + 10)       // 10 is added to the center x value to give a 10 pixel band to the target
      {  
        // calculate speed by multiplying the speedMultiplier to the distance of object from the center on the x-axis
        speed = speedMultiplierX * (x - highCenterX);

        // turn off right motor and set left motor velocity to the calculated speed to turn right
        leftMotor.setVelocity(speed, velocityUnits::pct);
        rightMotor.setVelocity(0, velocityUnits::pct);
      }
      // if the object's x-coordinate is to the right of the center of vision
      else if (x < highCenterX - 10)  // 10 is subtracted to the center x value to give a 10 pixel band to the target
      {      
        // calculate speed by multiplying the x-speedMultiplier to the distance of object from the center on the x-axis
        speed = speedMultiplierX * (highCenterX - x);

        // turn off left motor and set right motor velocity to the calculated speed to turn left
        leftMotor.setVelocity(0, velocityUnits::pct);
        rightMotor.setVelocity(speed, velocityUnits::pct);
      }
      // if object is in vision and centered on the x-axis
      else
      {
        // set motor velocities to 0 (stop motors)
        leftMotor.setVelocity(0, velocityUnits::pct);
        rightMotor.setVelocity(0, velocityUnits::pct);

        // end while loop
        //linedUp = true;
      }
      
      // spin both motors at set velocities
      rightMotor.spin(forward);
      leftMotor.spin(forward);

      // while loop delay (50 milliseconds)
      wait(50, msec);
    }
    return 0;
  }
};

// |-------------------- Function Definitions --------------------|

int drivetrainMotorsCallback()
{
  while (true)
  {
    leftMotor.spin(forward);
    rightMotor.spin(forward);

    printf("\nleft motor: %f | right motor: %f\n", leftMotor.velocity(velocityUnits::pct), rightMotor.velocity(velocityUnits::pct));

    wait(25, msec);
  }
  return 0;
}

// |------------------- Main --------------------|

int main()
{
  // Initializing Robot Configuration
  vexcodeInit();

  driveTrain dt;
  lift l;
  centerAssistTool cat;
  driveMotors = task(drivetrainMotorsCallback, vex::task::taskPriorityHigh);

  int count = 0;
  while (true)
  {
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("main has iterated %d times", count);
    count++;
    wait(25, msec);
  }
}

/*

CONTROLS:

leftJoyStick:    drive train (left + right motors)
rightJoyStick:   

buttonUp:        move lift up
buttonDown:      move lift down
buttonLeft:      
buttonRight:     

*/