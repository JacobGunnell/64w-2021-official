#include "ScoringSystem.h"

ScoringSystem::ScoringSystem(Motor &iBottomRollers, Motor &iTopRollers, MotorGroup &iIntakes, pros::ADILineSensor &iLowerLightSensor, pros::ADILineSensor &iUpperLightSensor, int iIntakeSpeed, int iOuttakeSpeed, int iRollerSpeed, int iLowerLightSensorThresholdLow, int iLowerLightSensorThresholdHigh, int iUpperLightSensorThresholdLow, int iUpperLightSensorThresholdHigh)
  : BottomRollers(iBottomRollers), TopRollers(iTopRollers), Intakes(iIntakes),
    LowerLightSensor(iLowerLightSensor), UpperLightSensor(iUpperLightSensor),
    intakeSpeed(iIntakeSpeed), outtakeSpeed(iOuttakeSpeed), rollerSpeed(iRollerSpeed),
    lowerLightSensorThresholdLow(iLowerLightSensorThresholdLow), lowerLightSensorThresholdHigh(iLowerLightSensorThresholdHigh), upperLightSensorThresholdLow(iUpperLightSensorThresholdLow), upperLightSensorThresholdHigh(iUpperLightSensorThresholdHigh),
    carriageCapacity(3), ballsInCarriage(0)
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

void ScoringSystem::grabSensor(QTime timeout, int numBalls, QTime extra)
{
  Timer timer;
  int targetBalls = getBallsInCarriage() + numBalls;
  QTime stopTime = timer.millis() + timeout - extra;
  grab();
  while(getBallsInCarriage() < targetBalls && timer.millis() < stopTime)
    pros::delay(50);
  pros::delay(extra.convert(millisecond));
  stop();
}

void ScoringSystem::scoreSensor(QTime timeout, int numBalls, QTime extra)
{
  Timer timer;
  int targetBalls;
  if(numBalls == -1) // Scores all balls in carriage by default
    targetBalls = 0;
  else
    targetBalls = getBallsInCarriage() - numBalls;
  QTime stopTime = timer.millis() + timeout - extra;
  if(getBallsInCarriage() > 1)
    split(200_ms);
  score();
  while(getBallsInCarriage() > targetBalls && timer.millis() < stopTime)
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
      if(BottomRollers.getActualVelocity() > 0 && ballsInCarriage < carriageCapacity) // Incoming balls are counted once the sensor reads low
      {
        ballsInCarriage++;
        std::cout << "Ball received from bottom, current count " << ballsInCarriage << std::endl;
      }
    }
    else if(lower && LowerLightSensor.get_value_calibrated() > lowerLightSensorThresholdHigh)
    {
      lower = false;
      if(BottomRollers.getActualVelocity() < 0 && ballsInCarriage > 0) // Outgoing balls are counted once the sensor reads high
      {
        ballsInCarriage--;
        std::cout << "Ball ejected out bottom, current count " << ballsInCarriage << std::endl;
      }
    }

    // Hysteresis for upper sensor
    if(!upper && UpperLightSensor.get_value_calibrated() < upperLightSensorThresholdLow)
    {
      upper = true;
      if(TopRollers.getActualVelocity() < 0 && ballsInCarriage < carriageCapacity) // Incoming balls are counted once the sensor reads low
      {
        ballsInCarriage++;
        std::cout << "Ball received from top, current count " << ballsInCarriage << std::endl;
      }
    }
    else if(upper && UpperLightSensor.get_value_calibrated() > upperLightSensorThresholdHigh)
    {
      upper = false;
      if(TopRollers.getActualVelocity() > 0 && ballsInCarriage > 0) // Outgoing balls are counted once the sensor reads high
      {
        ballsInCarriage--;
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
