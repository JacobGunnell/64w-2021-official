#ifndef AUTONBASE_H
#define AUTONBASE_H


#include "Hardware.h" // autonomous routines need access to robot hardware objects
#include <string>
#include <vector>
#include <memory>

constexpr QLength t = 23.75_in;

// Some nifty macros to abbreviate skills routines
#define START(x,y,h) Chassis->setState({x,y,h}); Imu.set_rotation(h.convert(degree)) // Set starting position
#define DTP(x,y) Chassis->driveToPoint({x,y}) // Drive To Point
#define DTPR(x,y) Chassis->driveToPoint({x,y}, true) // Drive To Point Reverse
#define TTP(x,y) Chassis->turnToPoint({x,y}) // Turn To Point
#define S(t) Scoring->score(t) // Score
#define AG(t, s) Gary->alignGoal(t, s) // Align Goal
#define GA(x,y) Gary->grabAt({x,y}) // Grab At

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
  void execWithBlackbox(Position);

  std::string getListing() const { return getName() + ": " + std::to_string(getPoints()); } // get listing as shown in a dropdown list

  static std::vector<std::unique_ptr<AutonBase>> getAllObjs();
  static std::string getAllListings();
  static void initAll();

private:
  static void blackboxLoggerCb();
};

#endif // AUTONBASE_H
