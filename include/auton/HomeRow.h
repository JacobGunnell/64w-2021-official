#pragma once


#include "AutonBase.h"

class HomeRow : public AutonBase
{
public:
  std::string getName() const override { return "Homerow"; }
  int getPoints() const override { return 0; } // TODO: how many points?
  std::string getDescription() const override { return "Starts at tile 2 [left] or 4 [right], completes the home row."; }
  void exec(Position p) override
  {
    // TODO: write some epic code
  }
};
