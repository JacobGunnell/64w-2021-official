#include "ScoringSystem.h"

ScoringSystem::ScoringSystem(Motor &iBottomRollers, Motor &iTopRollers, MotorGroup &iIntakes, pros::ADILineSensor &iLowerLightSensor, pros::ADILineSensor &iUpperLightSensor, int iIntakeSpeed, int iOuttakeSpeed, int iRollerSpeed, int iLowerLightSensorThresholdLow, int iLowerLightSensorThresholdHigh, int iUpperLightSensorThresholdLow, int iUpperLightSensorThresholdHigh)
  : BottomRollers(iBottomRollers), TopRollers(iTopRollers), Intakes(iIntakes),
    LowerLightSensor(iLowerLightSensor), UpperLightSensor(iUpperLightSensor),
    intakeSpeed(iIntakeSpeed), outtakeSpeed(iOuttakeSpeed), rollerSpeed(iRollerSpeed),
    lowerLightSensorThresholdLow(iLowerLightSensorThresholdLow), lowerLightSensorThresholdHigh(iLowerLightSensorThresholdHigh), upperLightSensorThresholdLow(iUpperLightSensorThresholdLow), upperLightSensorThresholdHigh(iUpperLightSensorThresholdHigh),
    carriageCapacity(3), ballsInCarriage(0), ballsScored(0), ballsGrabbed(0)
{
  carriageCounter = std::make_unique<pros::Task>(callCarriageCounterCb, static_cast<void *>(this));
}

// Basic Definitions

void ScoringSystem::deploy()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(rollerSpeed);
  pros::delay(500);
  stop();
}

void ScoringSystem::cycle()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(rollerSpeed);
  Intakes.moveVoltage(intakeSpeed);
}

void ScoringSystem::descore()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(-rollerSpeed/2);
  Intakes.moveVoltage(intakeSpeed);
}

void ScoringSystem::grab()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(rollerSpeed/6);
  Intakes.moveVoltage(intakeSpeed);
}

void ScoringSystem::flush()
{
  BottomRollers.moveVoltage(-rollerSpeed);
  Intakes.moveVoltage(-outtakeSpeed);
}

void ScoringSystem::score()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(rollerSpeed);
  Intakes.moveVoltage(0);
}

void ScoringSystem::eject()
{
  BottomRollers.moveVoltage(rollerSpeed);
  TopRollers.moveVoltage(-rollerSpeed/2);
  Intakes.moveVoltage(0);
}

void ScoringSystem::split()
{
  BottomRollers.moveVoltage(-rollerSpeed/2);
  TopRollers.moveVoltage(rollerSpeed/2);
  Intakes.moveVoltage(0);
}

void ScoringSystem::topOnly()
{
  BottomRollers.moveVoltage(0);
  TopRollers.moveVoltage(rollerSpeed);
  Intakes.moveVoltage(0);
}

void ScoringSystem::topOnlyReverse()
{
  BottomRollers.moveVoltage(0);
  TopRollers.moveVoltage(-rollerSpeed);
  Intakes.moveVoltage(0);
}

void ScoringSystem::intakesOnly()
{
  BottomRollers.moveVoltage(0);
  TopRollers.moveVoltage(0);
  Intakes.moveVoltage(intakeSpeed);
}

void ScoringSystem::intakesOnlyReverse()
{
  BottomRollers.moveVoltage(0);
  TopRollers.moveVoltage(0);
  Intakes.moveVoltage(-intakeSpeed);
}

void ScoringSystem::stop()
{
  BottomRollers.moveVoltage(0);
  TopRollers.moveVoltage(0);
  Intakes.moveVoltage(0);
}

// Smart Definitions

void ScoringSystem::grabSensor(int numBalls, QTime timeout, QTime extra)
{
  Timer timer;
  int targetBallsGrabbed = getBallsGrabbed() + numBalls;
  QTime stopTime = timer.millis() + timeout - extra;

  grab();
  while(getBallsGrabbed() < targetBallsGrabbed && timer.millis() < stopTime)
    pros::delay(50);
  pros::delay(extra.convert(millisecond));
  stop();
}

void ScoringSystem::scoreSensor(int numBalls, QTime timeout, QTime extra)
{
  Timer timer;
  int targetBallsScored;
  if(numBalls == -1) // Scores all balls in carriage by default
    targetBallsScored = getBallsScored() + getBallsInCarriage();
  else
    targetBallsScored = getBallsScored() + numBalls;
  QTime stopTime = timer.millis() + timeout - extra;

  if(getBallsInCarriage() > 1 && (numBalls > 1 || numBalls == -1)) // only split if carriage has 2 or more balls and 2 or more balls have been requested
    split(200_ms);
  score();
  while(getBallsScored() < targetBallsScored && timer.millis() < stopTime)
    pros::delay(50);
  pros::delay(extra.convert(millisecond));
  stop();
  setBallsInCarriage(0);
}

void ScoringSystem::flushSensor(QTime timeout, QTime extra)
{
  Timer timer;
  int targetBallsScored;
  QTime stopTime = timer.millis() + timeout - extra;

  flush();
  while(getBallsInCarriage() > 0 && timer.millis() < stopTime)
    pros::delay(50);
  pros::delay(extra.convert(millisecond));
  stop();
  setBallsInCarriage(0);
}

int ScoringSystem::getBallsInCarriage()
{
  carriageCountMtx.lock();
  int balls = ballsInCarriage;
  carriageCountMtx.unlock();
  return balls;
}

void ScoringSystem::setBallsInCarriage(int n)
{
  if(n >= 0 && n <= carriageCapacity)
  {
    carriageCountMtx.lock();
    ballsInCarriage = n;
    carriageCountMtx.unlock();
    std::cout << "Carriage Count manually set to " << n << std::endl;
  }
}

int ScoringSystem::getBallsGrabbed()
{
  carriageCountMtx.lock();
  int balls = ballsGrabbed;
  carriageCountMtx.unlock();
  return balls;
}

int ScoringSystem::getBallsScored()
{
  carriageCountMtx.lock();
  int balls = ballsScored;
  carriageCountMtx.unlock();
  return balls;
}

void ScoringSystem::carriageCounterCb()
{
  Rate r;
  bool lower = false, upper = false;

  while(true)
  {
    carriageCountMtx.lock();

    // Hysteresis for lower sensor
    if(!lower && LowerLightSensor.get_value_calibrated() < lowerLightSensorThresholdLow)
    {
      lower = true;
      // Incoming balls are counted once the sensor reads low
      if(BottomRollers.getActualVelocity() > 0 && ballsInCarriage < carriageCapacity)
      {
        ballsInCarriage++;
        ballsGrabbed++;
        std::cout << "Ball received from bottom, current count " << ballsInCarriage << std::endl;
      }
    }
    else if(lower && LowerLightSensor.get_value_calibrated() > lowerLightSensorThresholdHigh)
    {
      lower = false;
      // Outgoing balls are counted once the sensor reads high
      if(BottomRollers.getActualVelocity() < 0 && ballsInCarriage > 0)
      {
        ballsInCarriage--;
        std::cout << "Ball ejected out bottom, current count " << ballsInCarriage << std::endl;
      }
    }

    // Hysteresis for upper sensor
    if(!upper && UpperLightSensor.get_value_calibrated() < upperLightSensorThresholdLow)
    {
      upper = true;
      // Incoming balls are counted once the sensor reads low
      if(TopRollers.getActualVelocity() < 0 && ballsInCarriage < carriageCapacity)
      {
        ballsInCarriage++;
        std::cout << "Ball received from top, current count " << ballsInCarriage << std::endl;
      }
    }
    else if(upper && UpperLightSensor.get_value_calibrated() > upperLightSensorThresholdHigh)
    {
      upper = false;
      // Outgoing balls are counted once the sensor reads high
      if(TopRollers.getActualVelocity() > 0 && ballsInCarriage > 0)
      {
        ballsInCarriage--;
        ballsScored++;
        std::cout << "Ball ejected out top, current count " << ballsInCarriage << std::endl;
      }
    }

    carriageCountMtx.unlock();

    r.delay(20_Hz);
  }
}

void callCarriageCounterCb(void *s)
{
  static_cast<ScoringSystem *>(s)->carriageCounterCb();
}
