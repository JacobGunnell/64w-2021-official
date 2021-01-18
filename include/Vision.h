#ifndef VISION_H
#define VISION_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"
#include <vector>
#include <iostream>
#include <memory>

#define VISION_FOV_WIDTH_ANGLE 1.0654

enum Color {RED=1, BLUE=2};

template<std::size_t n> struct Ball
{
  Ball(int xi, int yi, int wi, int hi, Color c) : color(c) { x.filter(xi); y.filter(yi); w.filter(wi); h.filter(hi); }
  AverageFilter<n> x, y, w, h;
  Color color;
};

template<std::size_t n> class Vision : protected pros::Vision
{
public:
  Vision(int, int, pros::vision_signature_s_t, pros::vision_signature_s_t, float, float);
  const Ball<n> &operator[](int idx) const { return balls.at(idx); }
  const Ball<n> &at(int idx) const { return balls.at(idx); }
  int size() const { return balls.size(); }

  void update();

private:
  float getProjectedArea(pros::vision_object_s_t &);
  void print();

  std::vector<Ball<n>> balls;

  int minBallArea, maxBallArea; // min/max area for a thing to be considered a Ball
  int maxMotionToleranceSq; // square of the maximum distance a thing can more between frames for it to be considered the same thing

  float cameraHeight, depressionAngle; // all units are cm and rad
  float ballRadius;
};

template class Vision<10u>;
template class Vision<20u>;
template class Vision<30u>;

#endif // VISION_H
