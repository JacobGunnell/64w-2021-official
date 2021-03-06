#pragma once


#include "AutonBase.h"

class Illuminati : public AutonBase
{
public:
  std::string getName() const override { return "Illuminati"; }
  int getPoints() const override { return 75; } // in theory
  std::string getDescription() const override { return "Starts in tile 6 and scores in goals 1 4 7 8 9 & 5, forming a triangle."; }
  void exec(Position p) override
  {
    // Starting Position
    START(-2*t - 10.92_in, -2*t - 7.185_in, 25_deg);

    // Deploy bot, Score in goal 1, Grab balls at (-48, -36) and (0, -24)
    Scoring->topOnly(500_ms); // Flip top
    Drive->forward(.7);
    pros::delay(500);
    Gary->grabAtSensor({-.8*t, 0_in}, 2);

    // Score in goal 4
    Chassis->turnToAngle(-90_deg);
    Gary->alignGoalCamera(1.5_s, .5);
    Scoring->scoreSensor();
    Gary->backOut({-1.6*t, 0_in});

    // Grab ball at (-48, 36)
    Gary->grabAt({-2*t, 1.9*t});

    // Score in goal 7
    Chassis->turnToAngle(-45_deg);
    Gary->alignGoalCamera(.7_s, .5);
    Scoring->scoreSensor();
    Gary->backOut({-1.3*t, 1*t});

    // Grab balls at (0, 12) and (0, 24)
    Gary->grabAt({0_in, 1*t});
    Gary->grabAtSensor({0_in, 1.7*t}, 2);

    // Score in goal 8
    Chassis->turnToAngle(0_deg);
    Gary->alignGoalCamera(.7_s, .5);
    Scoring->scoreSensor();
    Gary->backOut({0_in, 1.5*t});

    // Grab ball at (48, 36)
    Chassis->turnToPoint({2*t, 1.5*t});
    Gary->grabAt({2*t, 1.5*t});
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);

    // Score in goal 9
    Chassis->turnToAngle(45_deg);
    Gary->alignGoalCamera(1.5_s, .5);
    Scoring->scoreSensor();
    Gary->backOut({1*t, 1.3*t});

    // Grab ball at (24, 0), score in goal 5
    Gary->grabAt({1*t, 4_in});
    Gary->alignGoal({1*t, 4_in}, 270_deg, 1_s, .4);
    Chassis->driveToPoint({1.2*t, 4_in}, true);
    Gary->alignGoal({1.2*t, 0_in}, 270_deg, .7_s, 0);
    Gary->alignGoal({1.2*t, 0_in}, 270_deg, 2_s, .3, true);
    Scoring->scoreSensor(1); // Don't allow the bot to accidentally score a blue ball
    Chassis->driveToPoint({1.5*t, 0_in}, true);
  }
};
