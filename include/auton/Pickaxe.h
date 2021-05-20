#pragma once


#include "AutonBase.h"

class Pickaxe : public AutonBase
{
public:
  std::string getName() const override { return "Pickaxe"; }
  int getPoints() const override { return 82; } // in theory
  std::string getDescription() const override { return "Starts in tile 6 and scores in goals 1 4 7 8 9 5 & 3, forming a pickaxe shape."; }
  void exec(Position p) override
  {
    // Starting Position
    //START(-2*t - 10.92_in, -2*t - 7.185_in, 25_deg);
    START(-2*t - 11.27_in + 1.5_in, -2*t - 7.19_in, 27_deg);

    // Deploy bot, Score in goal 1, Grab balls at (-48, -36) and (0, -24)
    Scoring->topOnly(500_ms); // Flip top
    Drive->forward(.7);
    pros::delay(500);
    Gary->grabAtSensor({-.8*t, -2_in}, 2);

    // Score in goal 4
    Chassis->turnToAngle(-90_deg);
    Gary->alignGoalCamera(1.5_s, .5);
    Gary->resetState({-2*t - 1.212_in, 0_in, -90_deg});
    Scoring->scoreSensor();
    Gary->backOut({-1.6*t, 0_in});

    // Grab ball at (-48, 36)
    Gary->grabAt({-2*t, 1.9*t});

    // Score in goal 7
    Chassis->turnToAngle(-45_deg);
    Gary->alignGoalCamera(.7_s, .5);
    Gary->resetState({-2*t - 5.305_in, 2*t + 6.6097_in, -45_deg});
    Scoring->scoreSensor(1);
    Gary->backOut({-1.3*t, 1*t});

    // Grab balls at (0, 12) and (0, 24)
    Gary->grabAt({0_in, 1*t});
    Gary->grabAtSensor({0_in, 1.7*t}, 1);

    // Score in goal 8
    Chassis->turnToAngle(0_deg);
    Gary->alignGoalCamera(.7_s, .5);
    Gary->resetState({0_in, 2*t + 2.7956_in, 0_deg});
    Scoring->scoreSensor();
    Gary->backOut({0_in, 1.5*t});

    // Grab ball at (48, 36)
    Chassis->turnToPoint({2*t, 1.5*t});
    Gary->grabAt({2*t, 1.5*t});
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);

    // Score in goal 9
    Chassis->turnToAngle(45_deg);
    Gary->alignGoalCamera(1.5_s, .5);
    //Gary->resetState({2*t + 5.7156_in, 2*t + 5.5487_in, 45_deg});
    Scoring->scoreSensor(1);
    Gary->backOut({1*t, 1.3*t});

    // Grab ball at (24, 0), score in goal 5
    Gary->grabAt({1*t, 4_in});
    Gary->alignGoal({1*t, 4_in}, 270_deg, 1_s, .4);
    Chassis->driveToPoint({1.2*t, 4_in}, true);
    Gary->alignGoal({1.2*t, 0_in}, 270_deg, .7_s, 0);
    Gary->alignGoal({1.2*t, 0_in}, 270_deg, 2_s, .3, true);
    Scoring->scoreSensor(1); // Don't allow the bot to accidentally score a blue ball
    Gary->backOut({1.6*t, 0_in});

    // Grab ball at (48, -36), score in goal 3
    Gary->grabAt({2*t, -1.6*t});
    Chassis->turnToAngle(135_deg);
    Gary->alignGoalCamera(.7_s, .5);
    Scoring->scoreSensor(1);
    Gary->backOut({1*t, -1*t});
  }
};
