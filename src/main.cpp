#include "main.h"
#include "GUI.h"
#include "Auton.h"

// Globals
// Motor objects


// Sensor objects
Controller Cont(ControllerId::master);

// Mutexes
pros::Mutex DriveMtx;

// Other Objects
std::shared_ptr<ChassisController> Chassis = ChassisControllerBuilder()
	.withMotors(1, -10, 11, -20)
	.withDimensions(AbstractMotor::gearset::green, {{4_in, 11.5_in}, imev5GreenTPR}) // TODO: update dimensions
	.build();
std::shared_ptr<XDriveModel> Drive = std::dynamic_pointer_cast<XDriveModel>(Chassis->getModel());

extern int autonNum;
extern Position position;


// Function Prototypes
void driveCtlCb();

// Function Implementations
void initialize()
{
	std::cout << "Robot Online" << std::endl << "64W Competition Program Rev. " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << VERSION_INFO << "-" << VERSION_YEAR << " with ";
	if (__cplusplus == 201703L) std::cout << "C++17\n";
  else if (__cplusplus == 201402L) std::cout << "C++14\n";
  else if (__cplusplus == 201103L) std::cout << "C++11\n";
  else if (__cplusplus == 199711L) std::cout << "C++98\n";
  else std::cout << "pre-standard C++\n";
	std::cout << "Begin Initialization" << std::endl;

	gui_loading_start();

	// Initialization stuff

	gui_loading_stop();
	gui_main();

	std::cout << "Initialization Complete" << std::endl;
}

void disabled()
{
	std::cout << "Robot disabled" << std::endl;
}

void competition_initialize() {} // TODO: should we make a seperate init for comps?

void autonomous()
{
	std::cout << "Executing autonomous #" << autonNum << "... ";
	AutonBase::getAllObjs()[autonNum]->exec(position);
	std::cout << "done" << std::endl;
}

void opcontrol()
{
	std::cout << "Entering operator control mode" << std::endl;
	pros::Task DriveCtl(driveCtlCb);
}

// Tasks

void driveCtlCb()
{
	while(true)
	{
		if(DriveMtx.take(20))
		{
			Drive->xArcade( // TODO: add dr/expo curves
	      Cont.getAnalog(ControllerAnalog::rightX),
	      Cont.getAnalog(ControllerAnalog::rightY),
	      Cont.getAnalog(ControllerAnalog::leftX));
			DriveMtx.give();
		}
    pros::delay(20);
	}
}
