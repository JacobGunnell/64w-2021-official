#pragma once


#include "AutonBase.h"

class NoAuton : public AutonBase // use this as a template for new autonomous routines; also, remember to add the new headers to Auton.h and AutonBase.cpp when you're finished
{
public:
  std::string getName() const override { return "No Auton"; } // name of routine here
  int getPoints() const override { return 0; } // points here
  std::string getDescription() const override { return ""; } // description here
  void exec(Position p) override
  {
    // autonomous code goes here
  }
};
