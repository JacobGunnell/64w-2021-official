#ifndef HOMEROW_H
#define HOMEROW_H


#include "AutonBase.h"

class HomeRow : public AutonBase
{
public:
  std::string getName() const override { return "Homerow"; }
  int getPoints() const override { return 0; } // TODO: how many points?
  void exec(Position p) override
  {
    // TODO: write some epic code
  }
};

#endif // HOMEROW_H
