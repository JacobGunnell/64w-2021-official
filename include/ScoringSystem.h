#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H


#include "okapi/api.hpp"
using namespace okapi;

class ScoringSystem
{
public:
  ScoringSystem(Motor &br, Motor &tr, MotorGroup &i, unsigned int intakespeed = 12000, unsigned int outtakespeed = 6000, unsigned int rollerspeed = 12000)
    : BottomRollers(br), TopRollers(tr), Intakes(i), intakeSpeed(intakespeed), outtakeSpeed(outtakespeed), rollerSpeed(rollerspeed) {}

  void deploy();
  void cycle();
  void descore();
  void grab();
  void flush();
  void score();
  void eject();
  void stop();

  void cycle(QTime t) { cycle(); r.delayUntil(t); stop(); }
  void descore(QTime t) { descore(); r.delayUntil(t); stop(); }
  void grab(QTime t) { grab(); r.delayUntil(t); stop(); }
  void flush(QTime t) { flush(); r.delayUntil(t); stop(); }
  void score(QTime t) { score(); r.delayUntil(t); stop(); }
  void eject(QTime t) { eject(); r.delayUntil(t); stop(); }

private:
  Motor &BottomRollers;
  Motor &TopRollers;
  MotorGroup &Intakes;

  unsigned int intakeSpeed, outtakeSpeed, rollerSpeed;

  Rate r; // just used for delayUntil
};

#endif // SCORINGSYSTEM_H
