#pragma once


#include "AutonBase.h"

class Skills159 : public AutonBase
{
public:
  std::string getName() const override { return "Skills* 159"; }
  int getPoints() const override { return 57; }
  std::string getDescription() const override { return "A hack of a programming skills that starts in tile 6 and scores in goals 1 5 & 9. Unfinished."; }
  void exec(Position p) override
  {
    // Starting Position
    Chassis->setState(OdomState{-3*t + 12_in, -54.5_in, 26_deg});

    // Deploy bot, score in goal 1
    Scoring->deploy();

    // Grab ball at (-48, -36)
    Scoring->grab();
    Chassis->driveToPoint({-26_in, -4_in}, false, 5_in);
    Scoring->stop();

    // Descore from goal 5
    Chassis->turnToPoint({-17_in, -4_in});
    Drive->forward(.5);
    pros::delay(2000);
    Drive->forward(-.5);
    pros::delay(1000);
    Drive->forward(.5);
    pros::delay(1000);
    Drive->forward(-.5);
    pros::delay(1000);
    Drive->forward(.5);
    pros::delay(1000);
    Drive->stop();

    // Score in goal 5
    Scoring->score(.5_s);
    Chassis->driveToPoint({-26_in, 0_in}, true);
  }
};
