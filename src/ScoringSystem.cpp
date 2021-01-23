#include "ScoringSystem.h"

void ScoringSystem::deploy()
{
  BottomRollers.moveVoltage(rollerSpeed);
  r.delayUntil(250_ms);
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
