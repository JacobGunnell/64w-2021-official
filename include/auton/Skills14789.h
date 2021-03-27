#pragma once


#include "AutonBase.h"

class Skills14789 : public AutonBase
{
public:
  std::string getName() const override { return "Skills* 14789"; }
  int getPoints() const override { return 65; }
  std::string getDescription() const override { return "Starts in tile 6 and scores in goals 1 4 7 8 & 9. Unfinished."; }
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
    Scoring->score(1_s); // TODO: scoresensor?
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
    Chassis->driveToPoint({-1.5*t, 1.2*t}, true);

    // Grab ball at (-36, 69) (nice)
    Scoring->grab();
    Chassis->driveToPoint({-1.5*t, 52_in});
    Chassis->driveToPoint({-1.5*t, 1.5*t}, true);
    Scoring->stop();

    // Score in goal 8
    while(Chassis->getState().x < -6_in)
    {
      Drive->strafe(-.5);
      pros::delay(50);
    }
    Drive->stop();
    Chassis->turnToAngle(0_deg);
    Drive->forward(1);
    pros::delay(1000);
    Drive->stop();
    Scoring->score(1_s);
    Chassis->driveToPoint({0_in, 2*t}, true);

    // Grab ball at (36, 69)
    Chassis->turnToAngle(0_deg);
    while(Chassis->getState().x < 1.5*t - 6_in)
    {
      Drive->strafe(-.5);
      pros::delay(50);
    }
    Drive->stop();
    Scoring->grab();
    Chassis->driveToPoint({1.5*t, 52_in});
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);
    Scoring->stop();

    // Score in goal 9
    Chassis->turnToPoint({3*t, 3*t});
    Drive->forward(1);
    pros::delay(1000);
    Drive->stop();
    Scoring->score(1_s);
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);
  }
};
