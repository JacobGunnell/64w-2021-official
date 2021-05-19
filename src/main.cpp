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
pros::Imu Imu(4);
std::shared_ptr<Vision<5>> Camera;
pros::ADILineSensor LowerLightSensor('G');
pros::ADILineSensor UpperLightSensor('H');

// Tasks / Mutexes
std::shared_ptr<pros::Task> DriveCtl, IntakeCtl, VisionTracking, OdomUpdater;
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
void odomUpdaterCb(void *);
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


	if(settings.enableImu)
	{
		std::cout << "Calibrating IMU... ";
		Imu.reset();
		while(Imu.is_calibrating())
			pros::delay(10);
		std::cout << "done" << std::endl;
	}


	std::cout << "Creating Chassis Control Objects... ";

	Logger::setDefaultLogger(std::make_shared<Logger>(TimeUtilFactory::createDefault().getTimer(), "/ser/sout", Logger::LogLevel::info));

	// PID Chassis
	Chassis = ChassisControllerBuilder() // TODO: try changing settling behavior?
		.withMotors(1, -10, -20, 11)
		.withGains(
			{.003, .0001, .00005}, // distance controller pid gains
			{.005, .003, 0}, // turn controller pid gains
			{.004, 0, .00005} // angle controller pid gains
		)
		.withDerivativeFilters(std::make_unique<AverageFilter<3>>())
		.withSensors(LeftQuad, RightQuad, MiddleQuad)
		.withDimensions(AbstractMotor::gearset::green, {{2.75_in, 11.125_in, 3.84_in, 2.75_in}, quadEncoderTPR})
		.withMaxVelocity(110)
		.withOdometry(StateMode::CARTESIAN)
		.buildOdometry();

	ProfileController = AsyncMotionProfileControllerBuilder()
		.withLimits({
	      1.0, // Maximum linear velocity of the Chassis in m/s
	      2.0, // Maximum linear acceleration of the Chassis in m/s/s
	      10.0 // Maximum linear jerk of the Chassis in m/s/s/s
	    })
		.withOutput(Chassis)
		.buildMotionProfileController();

	Drive = std::dynamic_pointer_cast<XDriveModel>(Chassis->getModel());

	Scoring = std::make_shared<ScoringSystem>(BottomRollers, TopRollers, Intakes, LowerLightSensor, UpperLightSensor);

	pros::vision_signature_s_t RED_BALL = pros::Vision::signature_from_utility (1, 6063, 9485, 7774, -2753, -327, -1540, 1.900, 0);
	pros::vision_signature_s_t BLUE_BALL = pros::Vision::signature_from_utility (2, -2545, -85, -1316, 897, 7427, 4162, 1.000, 0);
	pros::vision_signature_s_t GOAL = pros::Vision::signature_from_utility (3, -2823, -2405, -2614, -3993, -3385, -3689, 5.800, 0);
	Camera = std::make_shared<Vision<5>>(19, 50, RED_BALL, BLUE_BALL, GOAL, 0);

	Gary = std::make_shared<Robot>(Chassis, ProfileController, Scoring, Camera);

	std::cout << "done" << std::endl;


	std::cout << "Initializing all Autonomous Routines... ";
	AutonBase::initAll();
	std::cout << "done" << std::endl;


	std::cout << "Creating opcontrol Tasks... ";
	DriveCtl = std::make_shared<pros::Task>(driveCtlCb);
	IntakeCtl = std::make_shared<pros::Task>(intakeCtlCb);
	VisionTracking = std::make_shared<pros::Task>(visionTrackingCb);
	OdomUpdater = std::make_shared<pros::Task>(odomUpdaterCb);
	DriveCtl->suspend();
	IntakeCtl->suspend();
	VisionTracking->suspend();
	std::cout << "done" << std::endl;


	gui_loading_stop();
	gui_main();

	std::cout << "Initialization Complete" << std::endl;
}

void disabled()
{
	std::cout << "Robot disabled" << std::endl;

	// Stop all motors
	Drive->stop();
	Scoring->stop();

	// Suspend all opcontrol tasks
	DriveCtl->suspend();
	IntakeCtl->suspend();
	VisionTracking->suspend();

	// Unlock any locked mutexes
	DriveMtx.unlock();
	IntakeMtx.unlock();
}

void competition_initialize() {}

void autonomous()
{
	std::cout << "Executing autonomous #" << autonNum << "... ";
	std::cout.flush();

	Timer t;
	QTime start = t.millis();

	DriveMtx.lock();
	IntakeMtx.lock();
	if(settings.enableBlackbox)
		AutonBase::getAllObjs()[autonNum]->execWithBlackbox(position);
	else
		AutonBase::getAllObjs()[autonNum]->exec(position);
	DriveMtx.unlock();
	IntakeMtx.unlock();

	std::cout << "done in " << (t.millis() - start).convert(second) << " seconds" << std::endl;
}

void opcontrol()
{
	std::cout << "Entering operator control mode" << std::endl;
	DriveCtl->resume();
	IntakeCtl->resume();
	if(settings.enableVisionTracking)
		VisionTracking->resume();
}

// Task Callbacks
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

		if(Cont.getDigital(ControllerDigital::B))
		{
			START(0_in, 0_in, 0_deg);
		}
		else if(Cont.getDigital(ControllerDigital::Y))
		{
			START(-2*t - 10.92_in, -2*t - 7.185_in, 25_deg);
		}

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

		else if(Cont.getDigital(ControllerDigital::L1)) // Grab
			Scoring->grab();

		else if(Cont.getDigital(ControllerDigital::right)) // Intakes Only Reverse
			Scoring->intakesOnlyReverse();

		else if(Cont.getDigital(ControllerDigital::left)) // Top Only Reverse
			Scoring->topOnlyReverse();

		else if(Cont.getDigital(ControllerDigital::down)) // Autoscore (experimental)
			Scoring->scoreSensor();

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

void odomUpdaterCb(void *params)
{
	Rate r;
	if(settings.enableImu)
	{
		while(true)
		{
			auto state = Chassis->getState();
			state.theta = Imu.get_rotation() * 1.008 * degree; // IMU underestimates rotation by about .8%
			Chassis->setState(state);
			Cont.setText(2, 0, std::to_string(state.x.convert(inch)).substr(0,6) + " " + std::to_string(state.y.convert(inch)).substr(0,6) + " " + std::to_string(state.theta.convert(degree)).substr(0,6));
			r.delay(100_Hz);
		}
	}
	else
	{
		while(true)
		{
			auto state = Chassis->getState();
			Cont.setText(2, 0, std::to_string(state.x.convert(inch)).substr(0,6) + " " + std::to_string(state.y.convert(inch)).substr(0,6) + " " + std::to_string(state.theta.convert(degree)).substr(0,6));
			r.delay(10_Hz);
		}
	}
}

// Other
float drexpo(float input, double rgain, double egain) // Direct Rate / Expo function for joystick dampening
{
  return sgn(input)*rgain*pow(abs(input), 1+egain); // https://www.desmos.com/calculator/muxvne64c3
}
