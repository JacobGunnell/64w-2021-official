#ifndef ROBOT_H
#define ROBOT_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"
#include "Vision.h"
#include "ScoringSystem.h"
#include <vector>
#include <fstream>

class Robot
{
public:
  Robot(std::shared_ptr<OdomChassisController>,
    std::shared_ptr<AsyncMotionProfileController>,
    std::shared_ptr<ScoringSystem>,
    std::shared_ptr<Vision<5>>);

  std::shared_ptr<OdomChassisController> getChassis() const { return Chassis; }
  std::shared_ptr<AsyncMotionProfileController> getProfileController() const { return ProfileController; }
  std::shared_ptr<ScoringSystem> getScoring() const { return Scoring; }
  std::shared_ptr<Vision<5>> getCamera() const { return Camera; }

  void driveTime(QTime, double);
  void alignGoal(Point, QAngle, QTime, double, bool = false);
  void alignGoalCamera(QTime, double);
  void backOut(Point);
  void fetchBall(QTime, double);
  void grabAt(Point);
  void grabAtSensor(Point, int, QTime = 250_ms);
  void translateToPoint(Point, QAngle);
  bool resetState(OdomState, QLength = 3_in, QAngle = 7_deg); // TODO: make values more lenient?

  void logBlackboxFrame();
  bool saveBlackbox(std::string = "/usd/blackbox.csv");
  void clearBlackbox();

private:
  std::shared_ptr<OdomChassisController> Chassis;
  std::shared_ptr<XDriveModel> Drive;
  std::shared_ptr<AsyncMotionProfileController> ProfileController;
  std::shared_ptr<ScoringSystem> Scoring;
  std::shared_ptr<Vision<5>> Camera;

  std::vector<double> r_x;
  std::vector<double> r_y;
  std::vector<double> r_theta;
  std::vector<int> upperlight;
  std::vector<int> lowerlight;
  std::vector<int> cameraobjs;
};

#endif // ROBOT_H
