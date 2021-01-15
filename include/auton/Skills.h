#ifndef SKILLS_H
#define SKILLS_H


#include "AutonBase.h"

class Skills : public AutonBase
{
public:
  std::string getName() const override { return "Prog. Skills"; }
  int getPoints() const override { return 0; }
  std::string getDescription() const override { return "Starts at tile 2 and completes a one-minute skills autonomous."; }
  void exec(Position p) override
  {
    Chassis->setState(OdomState{-36_in, -60_in, 0_deg});
    //Scoring.deploy();
    //Scoring.grab(1_s);
    Chassis->driveToPoint(Point{-36_in, -36_in});
    Chassis->driveToPoint(Point{-48_in, -48_in});
    //Scoring.score(2_s);
    Chassis->driveToPoint(Point{-36_in, -36_in}, true);
  }
};

#endif // SKILLS_H
