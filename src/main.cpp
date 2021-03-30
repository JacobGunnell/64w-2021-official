#include "main.h"
#include "GUI.h"
#include "Auton.h"


// Motor Objects
Motor BottomRollers(2, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
Motor TopRollers(3, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees);
MotorGroup Intakes({Motor(8, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
										Motor(9, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees)});

// Sensor Objects
Controller Cont(ControllerId::master);
IMU Imu(4);
std::shared_ptr<Vision<10>> Camera;
pros::ADILineSensor LowerLightSensor('G');
pros::ADILineSensor UpperLightSensor('H');

// Mutexes
CrossplatformMutex DriveMtx, IntakeMtx;

// Facilities Control Objects
std::shared_ptr<OdomChassisController> Chassis;
std::shared_ptr<AsyncMotionProfileController> ProfileController;
std::shared_ptr<XDriveModel> Drive;
std::shared_ptr<ScoringSystem> Scoring;
std::shared_ptr<Robot> Gary;

extern int autonNum;
extern Position position;
extern Settings settings;

// Misc
template <typename T> int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}


// Function Prototypes
void driveCtlCb(void *);
void intakeCtlCb(void *);
void visionTrackingCb(void *);
float drexpo(float, double, double);

// Function Implementations
void initialize()
{
	std::cout << "Robot Online" << std::endl << "64W Competition Program Rev. " << VERSION_MAJOR << "." << VERSION_MINOR << VERSION_INFO << "-" << VERSION_YEAR << " with ";
	if (__cplusplus == 201703L) std::cout << "C++17\n";
  else if (__cplusplus == 201402L) std::cout << "C++14\n";
  else if (__cplusplus == 201103L) std::cout << "C++11\n";
  else if (__cplusplus == 199711L) std::cout << "C++98\n";
  else std::cout << "pre-standard C++\n";
	std::cout << "Begin Initialization" << std::endl;

	gui_loading_start();

	// Initialization stuff
	std::cout << "Creating Hardware Objects... ";

	auto LeftQuad = std::make_shared<ADIEncoder>('A', 'B'); // Quadrature encoders are *not* globals
	auto RightQuad = std::make_shared<ADIEncoder>('C', 'D', true);
	auto MiddleQuad = std::make_shared<ADIEncoder>('E', 'F');
	pros::delay(600);

	std::cout << "done" << std::endl;


	std::cout << "Loading Settings File... ";
	if(settings.load())
		std::cout << "success" << std::endl;
	else
		std::cout << "failure" << std::endl;


	std::cout << "Calibrating Light Sensors... ";
	LowerLightSensor.calibrate();
	UpperLightSensor.calibrate();
	std::cout << "done" << std::endl;


	if(settings.calibrateImuOnStart)
	{
		std::cout << "Calibrating IMU... ";
		Imu.calibrate();
		while(Imu.isCalibrating())
			pros::delay(10);
		std::cout << "done" << std::endl;
	}


	std::cout << "Creating Chassis Control Objects... ";

	Logger::setDefaultLogger(std::make_shared<Logger>(TimeUtilFactory::createDefault().getTimer(), "/ser/sout", Logger::LogLevel::debug));

	// PID-less Chassis
	/*Chassis = ChassisControllerBuilder()
		.withMotors(1, -10, -20, 11)
		.withDimensions(AbstractMotor::gearset::green, {{4_in, 13_in}, imev5GreenTPR})
		.withSensors(LeftQuad, RightQuad, MiddleQuad)
		.withMaxVelocity(50)
		.withOdometry({{2.75_in, 11.1_in, 4.3_in, 2.75_in}, quadEncoderTPR}, StateMode::CARTESIAN)
		.buildOdometry();*/

	// PID Chassis
	Chassis = ChassisControllerBuilder()
		.withMotors(1, -10, -20, 11)
		.withGains(
			{.002, .0001, .00005}, // distance controller pid gains
			{.008, 0, .0001}, // turn controller pid gains
			{.004, 0, 0} // angle controller pid gains
		)
		.withDerivativeFilters(std::make_unique<AverageFilter<3>>())
		.withSensors(LeftQuad, RightQuad, MiddleQuad)
		.withDimensions(AbstractMotor::gearset::green, {{2.75_in, 11.25_in, 3.84_in, 2.75_in}, quadEncoderTPR})
		.withMaxVelocity(100)
		.withOdometry(StateMode::CARTESIAN)
		.buildOdometry();

	Drive = std::dynamic_pointer_cast<XDriveModel>(Chassis->getModel());

	Scoring = std::make_shared<ScoringSystem>(BottomRollers, TopRollers, Intakes, LowerLightSensor, UpperLightSensor);

	pros::vision_signature_s_t RED_BALL = pros::Vision::signature_from_utility (1, 6063, 9485, 7774, -2753, -327, -1540, 1.900, 0);
	pros::vision_signature_s_t BLUE_BALL = pros::Vision::signature_from_utility (2, -2545, -85, -1316, 897, 7427, 4162, 1.000, 0);
	pros::vision_signature_s_t GOAL = pros::Vision::signature_from_utility (3, -3093, -1459, -2276, -4603, -2491, -3547, 2.000, 0);
	Camera = std::make_shared<Vision<10>>(19, 150, RED_BALL, BLUE_BALL, GOAL, 30, .26);

	Gary = std::make_shared<Robot>(Chassis, ProfileController, Scoring, Camera);

	std::cout << "done" << std::endl;


	std::cout << "Initializing all Autonomous Routines... ";
	AutonBase::initAll();
	std::cout << "done" << std::endl;


	gui_loading_stop();
	gui_main();

	std::cout << "Initialization Complete" << std::endl;
}

void disabled()
{
	std::cout << "Robot disabled" << std::endl;
	Drive->stop();
}

void competition_initialize() {} // TODO: should we make a seperate init for comps?

void autonomous()
{
	std::cout << "Executing autonomous #" << autonNum << "... ";
	std::cout.flush();
	DriveMtx.lock();
	IntakeMtx.lock();
	AutonBase::getAllObjs()[autonNum]->exec(position);
	DriveMtx.unlock();
	IntakeMtx.unlock();
	std::cout << "done" << std::endl;
}

void opcontrol()
{
	std::cout << "Entering operator control mode" << std::endl;
	pros::Task DriveCtl(&driveCtlCb, NULL);
	pros::Task IntakeCtl(&intakeCtlCb, NULL);
	if(settings.enableVisionTracking)
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
      drexpo(Cont.getAnalog(ControllerAnalog::leftX), 1.0, settings.translationalExpo),
      drexpo(Cont.getAnalog(ControllerAnalog::leftY) + Cont.getAnalog(ControllerAnalog::rightY), 1.0, settings.translationalExpo),
      drexpo(Cont.getAnalog(ControllerAnalog::rightX), settings.rotationalDR, settings.rotationalExpo));
		DriveMtx.unlock();

		auto state = Chassis->getState();
		//state.theta = Imu.get() * degree;
		//Chassis->setState(state);
		Cont.setText(2, 0, std::to_string(state.x.convert(inch)).substr(0,6) + " " + std::to_string(state.y.convert(inch)).substr(0,6) + " " + std::to_string(state.theta.convert(degree)).substr(0,6));

		if(Cont.getDigital(ControllerDigital::B))
		{
			Chassis->setState(OdomState{0_in, 0_in, 0_deg});
			//Imu.reset();
		}
		else if(Cont.getDigital(ControllerDigital::X))
			Chassis->setState(OdomState{-3*t + 12_in, -54.5_in, 26_deg});

		if(Cont.getDigital(ControllerDigital::Y))
			Chassis->turnToAngle(90_deg);

    r.delay(50_Hz);
	}
}

void intakeCtlCb(void *params)
{
	Rate r;

	while(true)
	{
		IntakeMtx.lock();

		if(Cont.getDigital(ControllerDigital::L2)) // Flush
			Scoring->flush();

		else if(Cont.getDigital(ControllerDigital::R1)) // Cycle
			Scoring->cycle();

		else if(Cont.getDigital(ControllerDigital::R2)) // Descore
			Scoring->descore();

		else if(Cont.getDigital(ControllerDigital::up)) // Score
			Scoring->score();

		else if(Cont.getDigital(ControllerDigital::down)) // Eject
			Scoring->eject();

		else if(Cont.getDigital(ControllerDigital::L1)) // Grab
			Scoring->grab();

		else if(Cont.getDigital(ControllerDigital::right)) // Intakes Only Reverse
		{
			BottomRollers.moveVoltage(0);
			TopRollers.moveVoltage(0);
			Intakes.moveVoltage(-12000);
		}
		else if(Cont.getDigital(ControllerDigital::left)) // Top Only Reverse
		{
			BottomRollers.moveVoltage(0);
			TopRollers.moveVoltage(-12000);
			Intakes.moveVoltage(0);
		}
		else
			Scoring->stop();

		IntakeMtx.unlock();

		r.delay(50_Hz);
	}
}

void visionTrackingCb(void *params)
{
	Rate r;

	while(true)
	{
		Camera->update();

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
				Camera->update();
				if(Camera->size() > 0)
				{
					auto target = (*Camera)[0];
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
