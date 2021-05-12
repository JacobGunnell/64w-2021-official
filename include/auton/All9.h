#pragma once


#include "AutonBase.h"

class All9 : public AutonBase
{
public:
  std::string getName() const override { return "All9*"; }
  int getPoints() const override { return 110; }
  std::string getDescription() const override { return "Starts in tile 6 and scores in every goal. Fricking OP. Also unfinished."; }
  void exec(Position p) override
  {
    // Starting Position
    START(-2*t - 10.92_in, -2*t - 7.185_in, 25_deg);


  }
};
