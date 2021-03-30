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
    std::shared_ptr<Vision<10>>);

  std::shared_ptr<OdomChassisController> getChassis() const { return Chassis; }
  std::shared_ptr<AsyncMotionProfileController> getProfileController() const { return ProfileController; }
  std::shared_ptr<ScoringSystem> getScoring() const { return Scoring; }
  std::shared_ptr<Vision<10>> getCamera() const { return Camera; }

  void driveTime(QTime, double);
  void alignGoal(QTime, double);
  void fetchBall(QTime, double);
  void strafeToPoint(Point);

  void logBlackboxFrame();
  bool saveBlackbox(std::string = "/usd/blackbox.csv");
  void clearBlackbox();

private:
  std::shared_ptr<OdomChassisController> Chassis;
  std::shared_ptr<XDriveModel> Drive;
  std::shared_ptr<AsyncMotionProfileController> ProfileController;
  std::shared_ptr<ScoringSystem> Scoring;
  std::shared_ptr<Vision<10>> Camera;

  //std::shared_ptr<OdomChassisController> StrafeChassis; // janky workaround to make okapilib do closed-loop strafes (TODO)

  std::vector<double> r_x;
  std::vector<double> r_y;
  std::vector<double> r_theta;
  std::vector<int> upperlight;
  std::vector<int> lowerlight;
  std::vector<int> cameraobjs;
};

#endif // ROBOT_H
