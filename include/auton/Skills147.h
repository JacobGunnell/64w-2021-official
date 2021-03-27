#pragma once


#include "AutonBase.h"

class Skills147 : public AutonBase
{
public:
  std::string getName() const override { return "Skills 147"; }
  int getPoints() const override { return 45; }
  std::string getDescription() const override { return "Starts in tile 6 and scores in goals 1 4 & 7."; }
  void exec(Position p) override
  {
    // Starting Position
    Chassis->setState(OdomState{-3*t + 12_in, -54.5_in, 26_deg});

    // Deploy bot, score in goal 1
    Scoring->deploy();

    // Grab ball at (-48, -36)
    Scoring->grab();
    Chassis->driveToPoint({-26_in, 0_in}, false, 5_in);
    Scoring->stop();

    // Score in goal 4
    Chassis->turnToPoint({-3*t, 0_in});
    Drive->forward(1);
    pros::delay(1000);
    Drive->stop();
    Scoring->score(1_s);
    Chassis->driveToPoint({-26_in, 0_in}, true);

    // Grab ball at (-48, 36)
    Scoring->grab();
    Chassis->driveToPoint({-2*t, 1.5*t}, false, -6_in);
    Scoring->stop();

    // Score in goal 7
    Chassis->turnToPoint({-3*t, 3*t});
    Drive->forward(1);
    pros::delay(1000);
    Drive->stop();
    Scoring->score(1_s);
    Chassis->driveToPoint({-1.5*t, 1.5*t}, true);
  }
};
