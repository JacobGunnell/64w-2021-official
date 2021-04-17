#pragma once


#include "AutonBase.h"

class Bagel : public AutonBase
{
public:
  std::string getName() const override { return "Bagel"; }
  int getPoints() const override { return 80; }
  std::string getDescription() const override { return "Starts in tile 6 and scores in goals 1 4 7 8 9 6 3 & 2."; }
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
    Gary->resetState({-2*t - 1.212_in, 0_in, -90_deg});
    Scoring->scoreSensor();
    Gary->backOut({-1.6*t, 0_in});

    // Grab ball at (-48, 36)
    Gary->grabAt({-2*t, 1.9*t});

    // Score in goal 7
    Chassis->turnToAngle(-45_deg);
    Gary->alignGoalCamera(.7_s, .5);
    Gary->resetState({-2*t - 5.305_in, 2*t + 6.6097_in, -45_deg});
    Scoring->scoreSensor();
    Gary->backOut({-1.3*t, .9*t});

    // Grab balls at (0, 12) and (0, 24)
    Gary->grabAt({0_in, .9*t});
    Gary->grabAtSensor({0_in, 1.7*t}, 2, 0_ms);

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
    Gary->resetState({2*t + 5.7156_in, 2*t + 5.5487_in, 45_deg});
    Scoring->scoreSensor();
    Gary->backOut({1*t, 1.3*t});

    // Grab ball at (24, 0), score in goal 6
    Gary->grabAt({1*t, 0_in});
    Chassis->turnToAngle(90_deg);
    Gary->alignGoalCamera(1.5_s, .5);
    Scoring->scoreSensor();
    Gary->backOut({1.5*t, 0_in});

    // Grab ball at (48, -36), score in goal 3
    Gary->grabAt({2*t, -1.5*t});
    Chassis->turnToAngle(135_deg);
    Gary->alignGoalCamera(1.5_s, .5);
    Scoring->scoreSensor();
    Gary->backOut({1*t, -1*t});

    // Grab balls at (0, -12) and (0, -24)
    Gary->grabAt({0_in, -.9*t});
    Gary->grabAtSensor({0_in, -1.7*t}, 2);

    // Score in goal 2
    Chassis->turnToAngle(180_deg);
    Gary->alignGoalCamera(.7_s, .5);
    Scoring->scoreSensor();
    Gary->backOut({0_in, -1.5*t});
  }
};
