#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"
#include <iostream>

class ScoringSystem
{
public:
  ScoringSystem(Motor &, Motor &, MotorGroup &, pros::ADILineSensor &, pros::ADILineSensor &, int = 12000, int = 6000, int = 12000, int = -150, int = -40, int = -100, int = -40);

  pros::ADILineSensor &getLowerLightSensor() const { return LowerLightSensor; }
  pros::ADILineSensor &getUpperLightSensor() const { return UpperLightSensor; }

  void deploy();
  void cycle();
  void descore();
  void grab();
  void flush();
  void score();
  void eject();
  void split();
  void topOnly();
  void topOnlyReverse();
  void intakesOnly();
  void intakesOnlyReverse();
  void stop();

  void cycle(QTime t) { cycle(); pros::delay(t.convert(millisecond)); stop(); }
  void descore(QTime t) { descore(); pros::delay(t.convert(millisecond)); stop(); }
  void grab(QTime t) { grab(); pros::delay(t.convert(millisecond)); stop(); }
  void flush(QTime t) { flush(); pros::delay(t.convert(millisecond)); stop(); }
  void score(QTime t) { score(); pros::delay(t.convert(millisecond)); stop(); setBallsInCarriage(0); }
  void eject(QTime t) { eject(); pros::delay(t.convert(millisecond)); stop(); }
  void split(QTime t) { split(); pros::delay(t.convert(millisecond)); stop(); }
  void topOnly(QTime t) { topOnly(); pros::delay(t.convert(millisecond)); stop(); }
  void topOnlyReverse(QTime t) { topOnlyReverse(); pros::delay(t.convert(millisecond)); stop(); }
  void intakesOnly(QTime t) { intakesOnly(); pros::delay(t.convert(millisecond)); stop(); }
  void intakesOnlyReverse(QTime t) { intakesOnlyReverse(); pros::delay(t.convert(millisecond)); stop(); }

  bool lowerSensorDetect() { return LowerLightSensor.get_value_calibrated() < lowerLightSensorThresholdLow; }
  bool upperSensorDetect() { return UpperLightSensor.get_value_calibrated() < upperLightSensorThresholdLow; }
  void grabSensor(int = 1, QTime = 2_s, QTime = 250_ms);
  void scoreSensor(int = -1, QTime = 2_s, QTime = 250_ms);
  void flushSensor(QTime = 2_s, QTime = 500_ms);

  int getBallsInCarriage();
  int getBallsGrabbed();
  int getBallsScored();

private:
  Motor &BottomRollers;
  Motor &TopRollers;
  MotorGroup &Intakes;

  pros::ADILineSensor &LowerLightSensor, &UpperLightSensor;


  int intakeSpeed, outtakeSpeed, rollerSpeed, lowerLightSensorThresholdLow, lowerLightSensorThresholdHigh, upperLightSensorThresholdLow, upperLightSensorThresholdHigh;
  int ballsInCarriage, ballsGrabbed, ballsScored;
  void setBallsInCarriage(int);
  const int carriageCapacity;

  std::unique_ptr<pros::Task> carriageCounter;
  void carriageCounterCb();
  friend void callCarriageCounterCb(void *);
  CrossplatformMutex carriageCountMtx;
};

void callCarriageCounterCb(void *);

#endif // SCORINGSYSTEM_H
