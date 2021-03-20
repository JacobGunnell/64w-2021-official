#pragma once


#include "AutonBase.h"

class Skills14 : public AutonBase
{
public:
  std::string getName() const override { return "Skills 14"; }
  int getPoints() const override { return 26; }
  std::string getDescription() const override { return "Starts at tile 4 and scores in goals 1 & 4."; }
  void exec(Position p) override
  {
    // Starting Position
    Chassis->setState(OdomState{-72_in + 12_in, -12_in, 152_deg});

    Scoring->deploy(); // Deploy bot, score in goal 4

    // Grab ball at -48, -36
    Scoring->grab();
    Chassis->driveToPoint({-46_in, -46_in});
    Scoring->stop();

    // Score in goal 1
    Chassis->turnToPoint({-54_in, -54_in});
    Drive->forward(1);
    pros::delay(500);
    Chassis->stop();
    Scoring->score(.5_s);
    Chassis->driveToPoint({-36_in, -36_in}, true);

/*
    // Grab ball at -36, -69 (nice)
    Scoring->grab();
    Chassis->driveToPoint({-36_in, -54_in});
    Chassis->driveToPoint({-36_in, -36_in}, true);
    Scoring->stop();

    Chassis->driveToPoint({0_in, -36_in}); // TODO: can we strafe to this point to save time?
    Chassis->turnToPoint({0_in, -52_in}); // TODO: bumper sensor
    Drive->forward(1);
    pros::delay(500);
    Chassis->stop();
    Scoring->score(.5_s);
    Chassis->driveToPoint({0_in, -36_in}, true);
  */
  }
};
