#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H


#include "Hardware.h"

class ScoringSystem
{
  cycle();
  descore();
  grab();
  flush();
  score();
  eject();
  stop();

  cycle(QTime t) { cycle(); pros::delay(t); stop(); }
  descore(QTime t) { descore(); pros::delay(t); stop(); }
  grab(QTime t) { grab(); pros::delay(t); stop(); }
  flush(QTime t) { flush(); pros::delay(t); stop(); }
  score(QTime t) { score(); pros::delay(t); stop(); }
  eject(QTime t) { eject(); pros::delay(t); stop(); }
};

#endif // SCORINGSYSTEM_H
