#ifndef HARDWARE_H
#define HARDWARE_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"
#include "Vision.h"

// Motor objects
extern Motor MainRollers;
extern Motor IndexingRoller;
extern MotorGroup Intakes;

// Sensor objects
extern Controller Cont;
extern IMU Imu;
extern Vision<25> Camera;

// Mutexes
extern CrossplatformMutex DriveMtx, IntakeMtx;

// Other objects
extern std::shared_ptr<OdomChassisController> Chassis;
extern std::shared_ptr<XDriveModel> Drive;

#endif // HARDWARE_H
