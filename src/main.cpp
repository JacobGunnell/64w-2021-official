#include "main.h"
#include "GUI.h"
#include "Auton.h"

// Globals
// Motor objects
Motor BottomRollers(2, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
Motor TopRollers(3, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
MotorGroup Intakes({Motor(8, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
										Motor(9, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees)});

// Sensor objects
Controller Cont(ControllerId::master);
IMU Imu(4, IMUAxes::x);
pros::vision_signature_s_t RED_BALL = pros::Vision::signature_from_utility (1, 6063, 9485, 7774, -2753, -327, -1540, 1.900, 0);
pros::vision_signature_s_t BLUE_BALL = pros::Vision::signature_from_utility (2, -2545, -85, -1316, 897, 7427, 4162, 1.000, 0);
Vision<10> Camera(19, 150, RED_BALL, BLUE_BALL, 30, .26);

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
void driveCtlCb(void *);
void intakeCtlCb(void *);
void tempCheckCb(void *);
void visionTrackingCb(void *);
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
	pros::Task DriveCtl(&driveCtlCb, NULL);
	pros::Task IntakeCtl(&intakeCtlCb, NULL);
	pros::Task TempCheck(&tempCheckCb, NULL);
	pros::Task VisionTracking(&visionTrackingCb, NULL);
}

// Tasks
void driveCtlCb(void *params)
{
	Rate r;

	while(true)
	{
		DriveMtx.lock();
		Drive->xArcade(
      drexpo(Cont.getAnalog(ControllerAnalog::leftX), 1.0, translationalExpo),
      drexpo(Cont.getAnalog(ControllerAnalog::leftY) + Cont.getAnalog(ControllerAnalog::rightY), 1.0, translationalExpo),
      drexpo(Cont.getAnalog(ControllerAnalog::rightX), rotationalDR, rotationalExpo));
		DriveMtx.unlock();

    r.delay(50_Hz);
	}
}

void intakeCtlCb(void *params)
{
	Rate r;

	int intakeSpeed = 12000,
			outtakeSpeed = 6000,
			rollerSpeed = 12000;

	while(true)
	{
		IntakeMtx.lock();
		if(Cont.getDigital(ControllerDigital::R1)) // Cycle
		{
			BottomRollers.moveVoltage(rollerSpeed);
			TopRollers.moveVoltage(rollerSpeed);
			Intakes.moveVoltage(intakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::R2)) // Descore
		{
			BottomRollers.moveVoltage(rollerSpeed);
			TopRollers.moveVoltage(-rollerSpeed/2);
			Intakes.moveVoltage(intakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::L2)) // Flush
		{
			BottomRollers.moveVoltage(-rollerSpeed);
			Intakes.moveVoltage(-outtakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::up)) // Score
		{
			BottomRollers.moveVoltage(rollerSpeed);
			TopRollers.moveVoltage(rollerSpeed);
			Intakes.moveVoltage(0);
		}
		else if(Cont.getDigital(ControllerDigital::down)) // Eject
		{
			BottomRollers.moveVoltage(rollerSpeed);
			TopRollers.moveVoltage(-rollerSpeed/2);
			Intakes.moveVoltage(0);
		}
		else if(Cont.getDigital(ControllerDigital::L1)) // Grab
		{
			BottomRollers.moveVoltage(rollerSpeed);
			TopRollers.moveVoltage(0);
			Intakes.moveVoltage(intakeSpeed);
		}
		else if(Cont.getDigital(ControllerDigital::right)) // Top Only Forward
		{
			BottomRollers.moveVoltage(0);
			TopRollers.moveVoltage(rollerSpeed);
			Intakes.moveVoltage(0);
		}
		else if(Cont.getDigital(ControllerDigital::left)) // Top Only Reverse
		{
			BottomRollers.moveVoltage(0);
			TopRollers.moveVoltage(-rollerSpeed);
			Intakes.moveVoltage(0);
		}
		else
		{
			BottomRollers.moveVoltage(0);
			TopRollers.moveVoltage(0);
			Intakes.moveVoltage(0);
		}
		IntakeMtx.unlock();

		r.delay(50_Hz);
	}
}

void tempCheckCb(void *params)
{
	Rate r;

	while(true)
	{
		if(TopRollers.isOverTemp())
		{
			Cont.rumble("-");
			pros::delay(200);
			Cont.setText(2, 0, "Over Temp!");
		}
		else
			Cont.clearLine(2);

		r.delay(.1_Hz);
	}
}

void visionTrackingCb(void *params)
{
	Rate r;

	while(true)
	{
		Camera.update();

		if(Cont.getDigital(ControllerDigital::A))
		{
			float turnGain = .05, speedRolloff = .005;
			float baseSpeed = .8;

			DriveMtx.lock();
			IntakeMtx.lock();

			Intakes.moveVoltage(12000);
			BottomRollers.moveVoltage(12000);
			while(Cont.getDigital(ControllerDigital::A))
			{
				Camera.update();
				if(Camera.size() > 0)
				{
					auto target = Camera[0];
					Drive->driveVector(baseSpeed - speedRolloff*target.y.getOutput(), turnGain*target.x.getOutput());
				}
				else
					Drive->driveVector(baseSpeed, 0);
			}
			Drive->stop();

			DriveMtx.unlock();
			IntakeMtx.unlock();
		}

		r.delay(10_Hz);
	}
}

// Other
float drexpo(float input, double rgain, double egain) // just a quick expo equation for the joystick controls
{
  return sgn(input)*rgain*pow(abs(input), 1+egain); // see Desmos for a pretty graph of this
}
