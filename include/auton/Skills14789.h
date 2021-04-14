#pragma once


#include "AutonBase.h"

class Skills14789 : public AutonBase
{
public:
  std::string getName() const override { return "Skills 14789"; }
  int getPoints() const override { return 65; }
  std::string getDescription() const override { return "Starts in tile 6 and scores in goals 1 4 7 8 & 9."; }
  void exec(Position p) override
  {
    // Starting Position
    Chassis->setState({-55.25_in, -53.75_in, 26_deg});
    Imu.set_rotation(26);

    // Deploy bot, Score in goal 1, Grab ball at (-48, -36)
    Scoring->topOnly(500_ms); // Flip top
    Drive->forward(.7);
    pros::delay(500);
    Scoring->grab();
    Chassis->driveToPoint({-1.6*t, 0_in});
    Scoring->stop();

    // Score in goal 4
    Chassis->turnToPoint({-3*t, 0_in});
    Gary->alignGoalCamera(1.5_s, .5);
    Scoring->score(1_s);
    Chassis->driveToPoint({-1.6*t, 0_in}, true);

    // Grab ball at (-48, 36)
    Scoring->grab();
    Chassis->driveToPoint({-2*t + 4_in, 1.5*t}, false, -6_in);
    Scoring->stop();

    // Score in goal 7
    Chassis->turnToPoint({-3*t, 3*t});
    Gary->alignGoalCamera(1.5_s, .5);
    Chassis->turnToPoint({-3*t, 3*t});
    Scoring->score(1_s);
    Chassis->driveToPoint({-2*t, 2*t}, true);

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
    Gary->alignGoalCamera(1.5_s, .5);
    Scoring->score(1_s);
    //auto goal = Camera->largest(GOAL);
    //if(goal != NULL && abs(goal->x.getOutput()) < .1*VISION_FOV_WIDTH) // TODO
    //auto state = Chassis->getState();
    //Chassis->setState({0_in, 50_in, state.theta}); // Reset odom state, but keep heading in case tabs didn't align
    Chassis->driveToPoint({0_in, 1.5*t}, true);

    // Grab ball at (48, 36)
    Chassis->turnToPoint({2*t, 1.5*t});
    Scoring->grab();
    Chassis->driveToPoint({2*t, 1.5*t});
    Scoring->stop();
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);

    // Score in goal 9
    Chassis->turnToPoint({3*t, 3*t});
    Gary->alignGoalCamera(1.5_s, .5);
    Chassis->turnToPoint({3*t, 3*t});
    Scoring->score(1_s);
    Chassis->driveToPoint({1.5*t, 1.5*t}, true);
  }
};
