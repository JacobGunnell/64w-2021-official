#ifndef HARDWARE_H
#define HARDWARE_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"
#include "Vision.h"
#include "ScoringSystem.h"
#include "Robot.h"

// Motor objects
extern Motor BottomRollers;
extern Motor TopRollers;
extern MotorGroup Intakes;

// Sensor objects
extern Controller Cont;
extern pros::Imu Imu;
extern std::shared_ptr<Vision<10>> Camera;
extern pros::ADILineSensor LowerLightSensor, UpperLightSensor;

// Tasks / Mutexes
extern std::shared_ptr<pros::Task> DriveCtl, IntakeCtl, VisionTracking, OdomUpdater;
extern CrossplatformMutex DriveMtx, IntakeMtx;

// Other objects
extern std::shared_ptr<OdomChassisController> Chassis;
extern std::shared_ptr<AsyncMotionProfileController> ProfileController;
extern std::shared_ptr<XDriveModel> Drive;
extern std::shared_ptr<ScoringSystem> Scoring;
extern std::shared_ptr<Robot> Gary;

#endif // HARDWARE_H
