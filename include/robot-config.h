using namespace vex;

extern brain Brain;

using signature = vision::signature;

// VEXcode devices
extern signature LOWREDGOAL;
extern signature LOWBLUEGOAL;
extern signature LOWYELLOWGOAL;

extern signature HIGHREDGOAL;
extern signature HIGHBLUEGOAL;
extern signature HIGHYELLOWGOAL;

extern vision highVision;
extern vision lowVision;

extern controller Controller1;
extern motor rightMotor;
extern motor leftMotor;

extern motor liftMotor;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );