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
    Chassis->setState({-2*t - 10.92_in, -2*t - 7.185_in, 25_deg});
    Imu.set_rotation(25);

    // Deploy bot, Score in goal 1, Grab ball at (-48, -36)
    Scoring->topOnly(500_ms); // Flip top
    Drive->forward(.7);
    pros::delay(500);
    Gary->grabAt({-1*t, 0_in});

    // Score in goal 4
    Chassis->turnToPoint({-3*t, 0_in});
    Gary->alignGoal(1.5_s, .5);
    Chassis->turnToPoint({-3*t + 6_in, 0_in});
    Scoring->score(1_s);
    Chassis->driveToPoint({-1.6*t, 0_in}, true);

    // Grab ball at (-48, 36)
    Gary->grabAt({-2*t, 1.9*t});

    // Score in goal 7
    Chassis->turnToPoint({-3*t, 3*t});
    Gary->alignGoal(1_s, .5);
    Chassis->turnToPoint({-3*t + 6_in, 3*t - 6_in});
    Scoring->score(1_s);
    Chassis->driveToPoint({-1.8*t, 2*t}, true);

    // Grab ball at (0, 24) and turn to face goal 8
    Chassis->turnToPoint({0_in, 2*t});
    Scoring->grab();
    Chassis->driveToPoint({0_in, 2*t});
    Scoring->stop();
    while(Chassis->getState().y > 1.5*t)
    {
      Drive->strafe(-.7);
      pros::delay(50);
    }
    Drive->stop();
    Chassis->turnToPoint({0_in, 3*t});

    // Score in goal 8
    Gary->alignGoal(1.5_s, .5);
    Chassis->turnToPoint({0_in, 3*t - 6_in});
    Scoring->score(1_s);
    //auto goal = Camera->largest(GOAL);
    //if(goal != NULL && abs(goal->x.getOutput()) < .1*VISION_FOV_WIDTH) // TODO
    //auto state = Chassis->getState();
    //Chassis->setState({0_in, 50_in, state.theta}); // Reset odom state, but keep heading in case tabs didn't align
    Chassis->driveToPoint({0_in, 1.5*t}, true);

    // Grab ball at (48, 36)
    Gary->grabAt({2*t, 1.5*t});
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);

    // Score in goal 9
    Chassis->turnToPoint({3*t, 3*t});
    Gary->alignGoal(1.5_s, .5);
    Chassis->turnToPoint({3*t - 6_in, 3*t - 6_in});
    Scoring->score(1_s);
    Chassis->driveToPoint({1*t, 1.3*t}, true);

    // Grab ball at (24, 0)
    Gary->grabAt({1*t, 0_in});

    // Score in goal 5
    Chassis->driveToPoint({1*t, 0_in});
    Chassis->turnToPoint({0_in, 0_in});
    Scoring->intakesOnly();
    Drive->forward(.4);
    pros::delay(1200);
    Drive->stop();
    Scoring->intakesOnlyReverse(4_s);
    Scoring->score(1_s);
    Chassis->driveToPoint({1.5*t, 0_in}, true);
  }
};
