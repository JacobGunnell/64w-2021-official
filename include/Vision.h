#ifndef VISION_H
#define VISION_H


#include "okapi/api.hpp"
using namespace okapi;
#include "pros/apix.h"
#include <vector>
#include <iostream>

#define VISION_FOV_WIDTH_ANGLE 1.0654

enum {RED_BALL=1, BLUE_BALL=2, GOAL=4, NONE=0};

template<std::size_t n> struct VisionObject
{
  VisionObject(int xi, int yi, int wi, int hi, unsigned char iType) : type(iType) { x.filter(xi); y.filter(yi); w.filter(wi); h.filter(hi); }
  AverageFilter<n> x, y, w, h;
  unsigned char type;
};

template<std::size_t n> class Vision : protected pros::Vision
{
public:
  Vision(int, int, pros::vision_signature_s_t, pros::vision_signature_s_t, pros::vision_signature_s_t, int = 0);
  const VisionObject<n> &operator[](int idx) const { return balls.at(idx); }
  const VisionObject<n> &at(int idx) const { return balls.at(idx); }
  int size() const { return balls.size(); }

  const VisionObject<n> *largest() { return largest(RED_BALL | BLUE_BALL | GOAL); }
  const VisionObject<n> *largest(unsigned char);

  void update();

private:
  void pushNewObject(int, int, int, int, unsigned char);
  void updateObject(typename std::vector<VisionObject<n>>::iterator, int, int, int, int);

  float getProjectedArea(pros::vision_object_s_t &);
  void print();

  std::vector<VisionObject<n>> balls;

  int minArea, maxArea; // min/max area for a thing to be considered a VisionObject
  int maxMotionToleranceSq; // square of the maximum distance a thing can more between frames for it to be considered the same thing

  int rotation; // 0 for 0 deg, 1, for 90 deg, 2 for 180 deg, and so forth
};

template class Vision<10u>;
template class Vision<20u>;
template class Vision<30u>;

#endif // VISION_H
