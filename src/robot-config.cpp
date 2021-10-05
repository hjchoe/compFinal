#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
signature LOWREDGOAL = signature (2, 4059, 9261, 6660, -1291, -209, -750, 1.4, 0);
signature LOWBLUEGOAL = signature (1, -2759, -857, -1808, 4095, 11947, 8021, 1.6, 0);
signature LOWYELLOWGOAL = signature (3, 307, 2377, 1342, -3777, -1619, -2698, 1.1, 0);

signature HIGHREDGOAL = signature (3, 6729, 9593, 8161, -1795, -885, -1340, 4.4, 0);
signature HIGHBLUEGOAL = signature (4, -3145, -1333, -2239, 5461, 11189, 8325, 2.2, 0);
signature HIGHYELLOWGOAL = signature (5, 149, 2571, 1360, -4359, -3459, -3909, 2.4, 0);

/*vex-vision-config:begin*/
vision lowVision = vision (PORT13, 50, LOWBLUEGOAL, LOWREDGOAL, LOWYELLOWGOAL);
/*vex-vision-config:end*/

/*vex-vision-config:begin*/
vision highVision = vision (PORT5, 50, HIGHBLUEGOAL, HIGHREDGOAL, HIGHYELLOWGOAL);
/*vex-vision-config:end*/

controller Controller1 = controller(primary);
motor rightMotor = motor(PORT11, ratio18_1, true);
motor leftMotor = motor(PORT14, ratio18_1, false);

motor liftMotor = motor(PORT10, ratio36_1, false);

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}