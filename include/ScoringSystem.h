#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"

class ScoringSystem
{
public:
  ScoringSystem(Motor &br, Motor &tr, MotorGroup &i, pros::ADILineSensor &l, pros::ADILineSensor &u, int intakespeed = 12000, int outtakespeed = 6000, int rollerspeed = 12000, int lowerLightSensorThreshold = -150, int upperLightSensorThreshold = -100)
    : BottomRollers(br), TopRollers(tr), Intakes(i), LowerLightSensor(l), UpperLightSensor(u), intakeSpeed(intakespeed), outtakeSpeed(outtakespeed), rollerSpeed(rollerspeed) {}

  pros::ADILineSensor &getLowerLightSensor() const { return LowerLightSensor; }
  pros::ADILineSensor &getUpperLightSensor() const { return UpperLightSensor; }

  void deploy();
  void cycle();
  void descore();
  void grab();
  void flush();
  void score();
  void eject();
  void topOnly();
  void topOnlyReverse();
  void intakesOnly();
  void intakesOnlyReverse();
  void stop();

  void cycle(QTime t) { cycle(); pros::delay(t.convert(millisecond)); stop(); }
  void descore(QTime t) { descore(); pros::delay(t.convert(millisecond)); stop(); }
  void grab(QTime t) { grab(); pros::delay(t.convert(millisecond)); stop(); }
  void flush(QTime t) { flush(); pros::delay(t.convert(millisecond)); stop(); }
  void score(QTime t) { score(); pros::delay(t.convert(millisecond)); stop(); }
  void eject(QTime t) { eject(); pros::delay(t.convert(millisecond)); stop(); }
  void topOnly(QTime t) { topOnly(); pros::delay(t.convert(millisecond)); stop(); }
  void topOnlyReverse(QTime t) { topOnlyReverse(); pros::delay(t.convert(millisecond)); stop(); }
  void intakesOnly(QTime t) { intakesOnly(); pros::delay(t.convert(millisecond)); stop(); }
  void intakesOnlyReverse(QTime t) { intakesOnlyReverse(); pros::delay(t.convert(millisecond)); stop(); }

  bool lowerSensorDetect() { return LowerLightSensor.get_value_calibrated() < lowerLightSensorThreshold; }
  bool upperSensorDetect() { return UpperLightSensor.get_value_calibrated() < upperLightSensorThreshold; }
  void grabSensor(QTime, QTime = 500_ms);
  void scoreSensor(QTime, QTime = 500_ms);

private:
  Motor &BottomRollers;
  Motor &TopRollers;
  MotorGroup &Intakes;

  pros::ADILineSensor &LowerLightSensor, &UpperLightSensor;

  int intakeSpeed, outtakeSpeed, rollerSpeed, lowerLightSensorThreshold, upperLightSensorThreshold;
};

#endif // SCORINGSYSTEM_H
