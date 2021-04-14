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
    OdomUpdater->suspend(); // Suspend task integrating IMU data until the rollers are done flipping out
    Scoring->topOnly(500_ms); // Flip top
    Drive->forward(.7);
    pros::delay(500);
    //Scoring->intakesOnlyReverse();
    //Scoring.BottomRollers.moveVoltage(12000);
    //pros::delay(250);
    Gary->grabAtSensor({-.8*t, 0_in}, 2);
    OdomUpdater->resume();

    // Score in goal 4
    Gary->alignGoal({-2*t, 0_in}, -90_deg, 1.5_s, .5);
    //Chassis->setState({-2*t - 1.212_in, 0_in, -90_deg});
    Scoring->scoreSensor(2_s);
    Gary->backOut({-1.6*t, 0_in});

    // Grab ball at (-48, 36)
    Gary->grabAt({-2*t, 1.9*t});

    // Score in goal 7
    Gary->alignGoal({-2*t, 2*t}, -45_deg, 1_s, .3);
    //Chassis->setState({-2*t - 5.305_in, 2*t + 6.6097_in, -45_deg});
    Scoring->scoreSensor(2_s);
    Gary->backOut({-1.3*t, .9*t});

    // Grab balls at (0, 12) and (0, 24)
    Gary->grabAt({0_in, .9*t});
    Gary->grabAtSensor({0_in, 1.7*t}, 2);

    // Score in goal 8
    Gary->alignGoal({0_in, 2*t}, 0_deg, 1_s, .3);
    //Chassis->setState({0_in, 2*t + 2.7956_in, 0_deg});
    Scoring->scoreSensor(2_s);
    Gary->backOut({0_in, 1.5*t});

    // Grab ball at (48, 36)
    Gary->grabAt({2*t, 1.5*t});
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);

    // Score in goal 9
    Gary->alignGoal({2*t, 2*t}, 45_deg, 1.5_s, .5);
    //Chassis->turnToAngle(45_deg);
    //Gary->alignGoalCamera(1.5_s, .5);
    //Chassis->setState({2*t + 5.7156_in, 2*t + 6.5487_in, 45_deg});
    Scoring->scoreSensor(2_s);
    Gary->backOut({1*t, 1.3*t});

/*
    // Grab ball at (24, 0)
    Gary->grabAt({1*t, 0_in});

    // Score in goal 5
    //Gary->alignGoal({1*t, 0_in}, 270_deg, 2_s, .2, true);
    //Chassis->driveToPoint({1*t, 0_in});
    Chassis->turnToAngle(270_deg);
    Scoring->intakesOnly();
    Drive->forward(.3);
    pros::delay(2000);
    Drive->stop();
    Scoring->intakesOnlyReverse(4_s);
    Chassis->turnToPoint({0_in, 0_in});
    Scoring->scoreSensor(2_s);
    Chassis->driveToPoint({1.5*t, 0_in}, true);
*/
    // Grab ball at (24, 0)
    Gary->grabAt({1*t, 4_in});
    Gary->alignGoal({1*t, 4_in}, 270_deg, 1_s, .4);
    Chassis->driveToPoint({1.2*t, 4_in}, true);
    Gary->alignGoal({1.2*t, 0_in}, 270_deg, 1_s, 0);
    Gary->alignGoal({1.2*t, 0_in}, 270_deg, 2_s, .3, true);
    Scoring->scoreSensor(2_s, 1); // Don't allow the bot to accidentally score a blue ball
    Chassis->driveToPoint({1.5*t, 0_in}, true);
  }
};
