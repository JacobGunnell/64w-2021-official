#pragma once


#include "AutonBase.h"

class Skills : public AutonBase
{
public:
  std::string getName() const override { return "Prog. Skills"; }
  int getPoints() const override { return 0; }
  std::string getDescription() const override { return "Starts at tile 4 and completes a one-minute skills autonomous."; }
  void init() override
  {
    ProfileController->generatePath({{-60_in, -36_in, 90_deg}, {-36_in, -36_in, 90_deg}}, "Skills-A");
  }
  void exec(Position p) override
  {
    Chassis->setState(OdomState{-60_in, -36_in, 90_deg});
    ProfileController->generatePath({{-60_in, -36_in, 90_deg}, {-36_in, -36_in, 90_deg}}, "Skills-A");
    /*Scoring.deploy();
    Scoring.grabSensor(1_s);
    Chassis->driveToPoint(Point{-36_in, -36_in});
    Chassis->driveToPoint(Point{-54_in, -54_in});
    Scoring.score(2_s);
    Chassis->driveToPoint(Point{-36_in, -36_in}, true);*/
    ProfileController->setTarget("Skills-A");
    ProfileController->waitUntilSettled();
  }
};
