#ifndef AUTONBASE_H
#define AUTONBASE_H


#include "Hardware.h" // autonomous routines need access to robot hardware objects
#include <string>
#include <vector>
#include <memory>

constexpr QLength t = 23.75_in; // tile size; TODO: do this more elegantly with an operator""

struct Position
{
  enum {RED, BLUE} side;
  enum {LEFT, RIGHT} pos;
};

class AutonBase
{
public:
  virtual ~AutonBase() = default;

  virtual std::string getName() const = 0;
  virtual int getPoints() const = 0;
  virtual std::string getDescription() const { return ""; }
  virtual void init() {}
  virtual void exec(Position) = 0;

  std::string getListing() const { return getName() + ": " + std::to_string(getPoints()); } // get listing as shown in a dropdown list

  static std::vector<std::unique_ptr<AutonBase>> getAllObjs();
  static std::string getAllListings();
  static void initAll();
};

#endif // AUTONBASE_H
