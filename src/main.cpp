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
// rightMotor           motor         11              
// leftMotor            motor         14   
// liftMotor            motor         10                      
// highVision           vision        5   
// lowVision            vision        13                 
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

bool canDrive = true;

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

  static int liftSpeed;

  lift()
  {
    liftSpeed = 50;
    Controller1.ButtonUp.pressed(liftUp);
    Controller1.ButtonDown.pressed(liftDown);
  }

  static void liftUp()
  {
    liftMotor.spinTo(liftAngle, rotationUnits::deg, liftSpeed, velocityUnits::pct, true);
  }

  static void liftDown()
  {
    liftMotor.spinTo(0, rotationUnits::deg, liftSpeed, velocityUnits::pct, true);
  }
};

// |-------------------- Function Definitions --------------------|

int driveMotorsCallback()
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

  task driveMotors = task(driveMotorsCallback, vex::task::taskPriorityHigh);

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

*/