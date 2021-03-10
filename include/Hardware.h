#ifndef HARDWARE_H
#define HARDWARE_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"
#include "Vision.h"
#include "ScoringSystem.h"

// Motor objects
extern Motor BottomRollers;
extern Motor TopRollers;
extern MotorGroup Intakes;

// Sensor objects
extern Controller Cont;
extern IMU Imu;
extern Vision<10> Camera;
extern pros::ADILineSensor LowerLightSensor;

// Mutexes
extern CrossplatformMutex DriveMtx, IntakeMtx;

// Other objects
extern std::shared_ptr<OdomChassisController> Chassis;
extern std::shared_ptr<AsyncMotionProfileController> ProfileController;
extern std::shared_ptr<XDriveModel> Drive;
extern ScoringSystem Scoring;

#endif // HARDWARE_H
