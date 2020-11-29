#include "main.h"
#include "GUI.h"
#include "Auton.h"

// Globals
// Motor objects
Motor MainRollers(2, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
Motor IndexingRoller(3, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
MotorGroup Intakes({Motor(8, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
										Motor(9, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees)});

// Sensor objects
Controller Cont(ControllerId::master);
IMU Imu(4, IMUAxes::x);

// Mutexes
CrossplatformMutex DriveMtx, IntakeMtx;

// Other Objects
std::shared_ptr<OdomChassisController> Chassis = ChassisControllerBuilder()
	.withMotors(1, -10, -20, 11)
	.withDimensions(AbstractMotor::gearset::green, {{4_in, 13_in}, imev5GreenTPR}) // TODO: update dimensions
	.withOdometry() // TODO: 3 encoder odometry?
	.buildOdometry(); // TODO: add PID gains?
std::shared_ptr<XDriveModel> Drive = std::dynamic_pointer_cast<XDriveModel>(Chassis->getModel());

extern int autonNum;
extern Position position;
extern double translationalExpo, rotationalDR, rotationalExpo;

// Misc
template <typename T> int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}


// Function Prototypes
void driveCtlCb();
void intakeCtlCb();
float drexpo(float, double, double);

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
	pros::Task IntakeCtl(intakeCtlCb);
}

// Tasks
void driveCtlCb()
{
	while(true)
	{
		DriveMtx.lock();
		Drive->xArcade(
      drexpo(Cont.getAnalog(ControllerAnalog::leftX), 1.0, translationalExpo),
      drexpo(Cont.getAnalog(ControllerAnalog::leftY) + Cont.getAnalog(ControllerAnalog::rightY), 1.0, translationalExpo),
      drexpo(Cont.getAnalog(ControllerAnalog::rightX), rotationalDR, rotationalExpo));
		DriveMtx.unlock();
    pros::delay(20);
	}
}

void intakeCtlCb()
{
	int intakeSpeed = 12000,
			outtakeSpeed = 6000,
			rollerSpeed = 12000;

	while(true)
	{
		IntakeMtx.lock();


		if(Cont.getDigital(ControllerDigital::R1)) // Cycle
		{
			MainRollers.moveVoltage(rollerSpeed);
			IndexingRoller.moveVoltage(rollerSpeed);
			Intakes.moveVoltage(intakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::R2)) // Descore
		{
			MainRollers.moveVoltage(rollerSpeed);
			IndexingRoller.moveVoltage(-rollerSpeed);
			Intakes.moveVoltage(intakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::L2)) // Flush
		{
			MainRollers.moveVoltage(-rollerSpeed);
			IndexingRoller.moveVoltage(-rollerSpeed);
			Intakes.moveVoltage(-outtakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::up)) // Score
		{
			MainRollers.moveVoltage(rollerSpeed);
			IndexingRoller.moveVoltage(rollerSpeed);
			Intakes.moveVoltage(0);
		}
		else if(Cont.getDigital(ControllerDigital::down)) // Eject
		{
			MainRollers.moveVoltage(rollerSpeed);
			IndexingRoller.moveVoltage(-rollerSpeed);
			Intakes.moveVoltage(0);
		}
		else if(Cont.getDigital(ControllerDigital::L1)) // Grab
		{
			MainRollers.moveVoltage(rollerSpeed);
			IndexingRoller.moveVoltage(0);
			Intakes.moveVoltage(intakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::right)) // Intake
		{
			MainRollers.moveVoltage(0);
			IndexingRoller.moveVoltage(0);
			Intakes.moveVoltage(intakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::left)) // Outtake
		{
			MainRollers.moveVoltage(0);
			IndexingRoller.moveVoltage(0);
			Intakes.moveVoltage(-outtakeSpeed);
		}
		else
		{
			MainRollers.moveVoltage(0);
			IndexingRoller.moveVoltage(0);
			Intakes.moveVoltage(0);
		}

		IntakeMtx.unlock();
		pros::delay(20);
	}
}

// Other
float drexpo(float input, double rgain, double egain) // just a quick expo equation for the joystick controls
{
    return sgn(input)*rgain*pow(abs(input), 1+egain); // see Desmos for a pretty graph of this
}
