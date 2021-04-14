#include "Vision.h"

template<std::size_t n> Vision<n>::Vision(int port, int exposure, pros::vision_signature_s_t redsig, pros::vision_signature_s_t bluesig, pros::vision_signature_s_t goalsig, int iRotation) : pros::Vision(port), rotation(iRotation)
{
  set_exposure(exposure);
  set_signature(RED_BALL, &redsig);
  set_signature(BLUE_BALL, &bluesig);
  set_signature(GOAL, &goalsig);
  set_zero_point(pros::vision_zero_e_t::E_VISION_ZERO_CENTER);
  minArea = 10;
  maxArea = 40*40; // very lenient values (this is mostly to filter out the little blips the camera catches that definitely aren't VisionObjects)
  maxMotionToleranceSq = 16*16;
}

template<std::size_t n> const VisionObject<n> *Vision<n>::largest(unsigned char iType)
{
  for(const auto &ball : balls)
    if(ball.type & iType)
      return &ball;
  return NULL;
}

template<std::size_t n> void Vision<n>::update()
{
  // get array from pros vision api
  const int SIZE = 8; // max objects the robot will be able to see at once
  pros::vision_object_s_t arr[SIZE];
  int count = read_by_size(0, SIZE, arr);
  if(count > SIZE) // if it returns an error, quit
    return;

  // compute approximate projected ball areas and take note of all non-VisionObject objects in a parallel array
  bool withinAreaBounds[count];
  for(int i = 0; i < count; i++)
  {
    float projectedArea = getProjectedArea(arr[i]);
    withinAreaBounds[i] = (projectedArea >= minArea && projectedArea <= maxArea);
  }

  // remove any existing balls not present in current snapshot
  for(typename std::vector<VisionObject<n>>::iterator it = balls.begin(); it != balls.end(); )
  {
    bool match = false;
    for(int i = 0; i < count; i++)
    {
      int dx = it->x.getOutput() - arr[i].x_middle_coord;
      int dy = it->y.getOutput() - arr[i].y_middle_coord;
      if(withinAreaBounds[i] && arr[i].signature == it->type && dx*dx + dy*dy <= maxMotionToleranceSq) // if it's the same type and it didn't move too far since last frame
      {
        match = true;
        break;
      }
    }
    if(!match)
      it = balls.erase(it);
    else
      ++it;
  }

  // add any new balls, update existing
  for(int i = 0; i < count; i++)
  {
    if(withinAreaBounds[i])
    {
      bool match = false;
      for(typename std::vector<VisionObject<n>>::iterator it = balls.begin(); it != balls.end(); ++it)
      {
        int dx = it->x.getOutput() - arr[i].x_middle_coord;
        int dy = it->y.getOutput() - arr[i].y_middle_coord;
        if(withinAreaBounds[i] && arr[i].signature == it->type && dx*dx + dy*dy <= maxMotionToleranceSq) // if it's the same type and it didn't move too far since last frame
        {
          it->x.filter(arr[i].x_middle_coord);
          it->y.filter(arr[i].y_middle_coord);
          it->w.filter(arr[i].width);
          it->h.filter(arr[i].height);
          match = true;
          break;
        }
      }
      if(!match) // if no match was found, create a new object in balls
        pushNewObject(arr[i].x_middle_coord, arr[i].y_middle_coord, arr[i].width, arr[i].height, arr[i].signature);
    }
  }
}

template<std::size_t n> void Vision<n>::pushNewObject(int x, int y, int w, int h, unsigned char t)
{
  switch(rotation)
  {
  case 1: // +90 degrees
    balls.push_back(VisionObject<n>(y, -x, h, w, t));
    break;
  case 2: // +180 degrees
    balls.push_back(VisionObject<n>(-x, -y, w, h, t));
    break;
  case 3: // -90 degrees
    balls.push_back(VisionObject<n>(-y, x, h, w, t));
    break;
  default: // +0 degrees
    balls.push_back(VisionObject<n>(x, y, w, h, t));
  }
}

template<std::size_t n> void Vision<n>::updateObject(typename std::vector<VisionObject<n>>::iterator it, int x, int y, int w, int h)
{
  switch(rotation)
  {
  case 1: // +90 degrees
    it->x.filter(y);
    it->y.filter(-x);
    it->w.filter(h);
    it->h.filter(w);
    break;
  case 2: // +180 degrees
    it->x.filter(-x);
    it->y.filter(-y);
    it->w.filter(w);
    it->h.filter(h);
    break;
  case 3: // -90 degrees
    it->x.filter(-y);
    it->y.filter(x);
    it->w.filter(h);
    it->h.filter(w);
    break;
  default: // +0 degrees
    it->x.filter(x);
    it->y.filter(y);
    it->w.filter(w);
    it->h.filter(h);
  }
}

template<std::size_t n> float Vision<n>::getProjectedArea(pros::vision_object_s_t &obj)
{
  return obj.width * obj.height;
}

template<std::size_t n> void Vision<n>::print()
{
  std::cout << "balls array size " << balls.size() << " with contents:" << std::endl;
  for(typename std::vector<VisionObject<n>>::iterator it = balls.begin(); it != balls.end(); ++it)
  {
    if(it->type == RED_BALL)
      std::cout << "red ball ";
    else if(it->type == BLUE_BALL)
      std::cout << "blue ball ";
    else if(it->type == GOAL)
      std::cout << "goal ";
    std::cout << "at " << it->x.getOutput() << "," << it->y.getOutput() << std::endl;
  }
}
