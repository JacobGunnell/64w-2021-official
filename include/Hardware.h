#ifndef HARDWARE_H
#define HARDWARE_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"

// Motor objects


// Sensor objects
extern Controller Cont;

// Mutexes
extern pros::Mutex DriveMtx;

// Other objects
extern std::shared_ptr<OdomChassisController> Chassis;
extern std::shared_ptr<XDriveModel> Drive;

#endif // HARDWARE_H
