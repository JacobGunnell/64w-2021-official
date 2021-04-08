#include "Robot.h"

Robot::Robot(std::shared_ptr<OdomChassisController> chassis,
  std::shared_ptr<AsyncMotionProfileController> profileController,
  std::shared_ptr<ScoringSystem> scoring,
  std::shared_ptr<Vision<10>> camera) : Chassis(chassis), ProfileController(profileController), Scoring(scoring), Camera(camera)
{
  Drive = std::dynamic_pointer_cast<XDriveModel>(Chassis->getModel());

  r_x.reserve(600);
  r_y.reserve(600);
  r_theta.reserve(600);
  upperlight.reserve(600);
  lowerlight.reserve(600);
  cameraobjs.reserve(600);
}

void Robot::driveTime(QTime t, double power)
{
  Drive->forward(power);
  pros::delay(t.convert(millisecond));
  Drive->stop();
}

void Robot::alignGoal(QTime timeout, double power)
{
  Timer timer;
  Rate r;
  QTime start = timer.millis();

  double strafeGain = .05;
  QFrequency f = 10_Hz;

  while(timer.millis() < start + timeout) // TODO: add bumper sensor
  {
    Camera->update();
    auto target = Camera->largest(GOAL);
    if(target != NULL)
      Drive->xArcade(strafeGain*target->x.getOutput(), power, 0);
    else
      Drive->xArcade(0, power, 0);
    r.delay(f);
  }
  Drive->stop();
}

void Robot::fetchBall(QTime timeout, double power)
{
  Timer timer;
  Rate r;
  QTime start = timer.millis();
  Point startingPoint = {Chassis->getState().x, Chassis->getState().y};

  Scoring->grab();
  while(timer.millis() < start + timeout && !Scoring->lowerSensorDetect())
  {
    Drive->forward(power);
    r.delay(10_Hz);
  }
  Chassis->driveToPoint(startingPoint, true);
  Scoring->stop();
}

void Robot::grabAt(Point p) // TODO: vision alignment?
{
  Chassis->turnToPoint(p);
  Scoring->grab();
  Chassis->driveToPoint(p);
  Scoring->stop();
}

void Robot::strafeToPoint(Point target)
{
  // TODO
}

void Robot::logBlackboxFrame()
{
  auto state = Chassis->getState();
  r_x.push_back(state.x.convert(inch));
  r_y.push_back(state.y.convert(inch));
  r_theta.push_back(state.theta.convert(degree));

  upperlight.push_back(Scoring->getUpperLightSensor().get_value_calibrated());
  lowerlight.push_back(Scoring->getLowerLightSensor().get_value_calibrated());

  cameraobjs.push_back(Camera->size());
}

bool Robot::saveBlackbox(std::string filename) // TODO: make this default to saving into a new file (i.e. blackbox22.csv)
{
  std::ofstream file(filename, std::ofstream::trunc);
  if(file)
  {
    for(const auto &el : r_x)
      file << el << ',';
    file << std::endl;
    for(const auto &el : r_y)
      file << el << ',';
    file << std::endl;
    for(const auto &el : r_theta)
      file << el << ',';
    file << std::endl;
    for(const auto &el : upperlight)
      file << el << ',';
    file << std::endl;
    for(const auto &el : lowerlight)
      file << el << ',';
    file << std::endl;
    for(const auto &el : cameraobjs)
      file << el << ',';
    file.close();
    return true;
  }
  else
  {
    file.close();
    return false;
  }
}

void Robot::clearBlackbox()
{
  r_x.clear();
  r_y.clear();
  r_theta.clear();
  upperlight.clear();
  lowerlight.clear();
  cameraobjs.clear();
}
