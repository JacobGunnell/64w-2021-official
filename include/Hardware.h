#ifndef HARDWARE_H
#define HARDWARE_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"

// Motor objects
extern Motor IntakeU;
extern Motor IntakeL;

// Sensor objects
extern Controller Cont;
extern IMU Imu;

// Mutexes
extern CrossplatformMutex DriveMtx, IntakeMtx;

// Other objects
extern std::shared_ptr<OdomChassisController> Chassis;
extern std::shared_ptr<XDriveModel> Drive;

#endif // HARDWARE_H
