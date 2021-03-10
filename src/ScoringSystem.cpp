#include "ScoringSystem.h"

// Basic Definitions

void ScoringSystem::deploy()
{
  BottomRollers.moveVoltage(rollerSpeed);
  pros::delay(500); // TODO: use imu to sense when robot has successfully deployed
  BottomRollers.moveVoltage(0);
}

void ScoringSystem::cycle()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(rollerSpeed);
  Intakes.moveVoltage(intakeSpeed);
}

void ScoringSystem::descore()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(-rollerSpeed/2);
  Intakes.moveVoltage(intakeSpeed);
}

void ScoringSystem::grab()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(0);
  Intakes.moveVoltage(intakeSpeed);
}

void ScoringSystem::flush()
{
  BottomRollers.moveVoltage(-rollerSpeed);
  Intakes.moveVoltage(-outtakeSpeed);
}

void ScoringSystem::score()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(rollerSpeed);
  Intakes.moveVoltage(0);
}

void ScoringSystem::eject()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(-rollerSpeed/2);
  Intakes.moveVoltage(0);
}

void ScoringSystem::stop()
{
  BottomRollers.moveVoltage(0);
  TopRollers.moveVoltage(0);
  Intakes.moveVoltage(0);
}

// Smart Definitions

void ScoringSystem::grabSensor(QTime t)
{
  Rate r;
  Timer timer;
  QTime stopTime = timer.millis() + t;
  while(LowerLightSensor.get_value() > lightSensorThreshold && timer.millis() < stopTime) // TODO: get_value_calibrated?
  {
    grab();
    r.delay(10_Hz);
  }
  stop();
}
