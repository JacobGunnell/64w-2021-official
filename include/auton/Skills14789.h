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
    Chassis->setState({-3*t + 12_in, -54.5_in, 26_deg});

    // Deploy bot, score in goal 1
    Scoring->deploy();

    // Grab ball at (-48, -36)
    Scoring->grab();
    Chassis->driveToPoint({-26_in, 0_in}, false, 5_in);
    Scoring->stop();

    // Score in goal 4
    Chassis->turnToPoint({-3*t, 0_in});
    Gary->alignGoal(1.5_s, .5);
    Scoring->scoreSensor(2_s);
    Chassis->driveToPoint({-26_in, 0_in}, true);

    // Grab ball at (-48, 36)
    Scoring->grab();
    Chassis->driveToPoint({-2*t, 1.5*t}, false, -6_in);
    Scoring->stop();

    // Score in goal 7
    Chassis->turnToPoint({-3*t, 3*t});
    Gary->alignGoal(1.5_s, .5);
    Scoring->scoreSensor(2_s);
    Chassis->driveToPoint({-1.5*t, 1.2*t}, true);

    // Grab ball at (-36, 69) (nice)
    Chassis->turnToAngle(0_deg);
    Gary->fetchBall(2_s, .7);

    // Score in goal 8
    while(Chassis->getState().x < -6_in)
    {
      Drive->strafe(-.5);
      pros::delay(50);
    }
    Drive->stop();
    Chassis->turnToAngle(0_deg);
    Gary->alignGoal(1.5_s, .5);
    Scoring->scoreSensor(2_s);
    //auto goal = Camera->largest(GOAL);
    //if(goal != NULL && abs(goal->x.getOutput()) < .1*VISION_FOV_WIDTH) // TODO
    Chassis->setState({0_in, 50_in, 0_deg});
    Chassis->driveToPoint({0_in, 1.5*t}, true);

    // Grab ball at (36, 69)
    while(Chassis->getState().x < 1.5*t)
    {
      Drive->strafe(-.5);
      pros::delay(50);
    }
    Drive->stop();
    Chassis->turnToAngle(0_deg);
    Gary->fetchBall(2_s, .7);

    // Score in goal 9
    Chassis->turnToPoint({3*t, 3*t});
    Gary->alignGoal(1.5_s, .5);
    Scoring->scoreSensor(2_s);
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);
  }
};
