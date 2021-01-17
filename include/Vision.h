#ifndef VISION_H
#define VISION_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"
#include <vector>
#include <iostream>
#include <memory>

enum Color {RED=1, BLUE=2};

template<std::size_t n> struct Ball
{
  Ball(int xi, int yi, int c) : color(static_cast<Color>(c)) { x.filter(xi); y.filter(yi); }
  AverageFilter<n> x;
  AverageFilter<n> y;
  Color color;
};

template<std::size_t n> class Vision : protected pros::Vision
{
public:
  Vision(int, int, pros::vision_signature_s_t, pros::vision_signature_s_t);
  const Ball<n> &operator[](int idx) const { return balls.at(idx); }
  const Ball<n> &at(int idx) const { return balls.at(idx); }
  int size() const { return balls.size(); }

  void update();

private:
  void print();

  std::vector<Ball<n>> balls;
  int minBallArea; // minimum area for a thing to be considered a Ball
  int maxFrameToleranceSq; // square of the maximum distance a thing can more between frames for it to be considered the same thing
};

template class Vision<10u>;
template class Vision<20u>;
template class Vision<30u>;

#endif // VISION_H
