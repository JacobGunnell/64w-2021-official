#pragma once


#include "AutonBase.h"

class NoAuton : public AutonBase // use this as a template for new autonomous routines; also, remember to add the new headers to Auton.h and AutonBase.cpp when you're finished
{
public:
  std::string getName() const override { return "Skills* 147"; } // name of routine here
  int getPoints() const override { return 0; } // points here
  std::string getDescription() const override { return ""; } // description here
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
  }
};
