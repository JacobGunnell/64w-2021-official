#include "Vision.h"

template<std::size_t n> Vision<n>::Vision(int port, int exposure, pros::vision_signature_s_t redsig, pros::vision_signature_s_t bluesig, float cameraHeight_, float depressionAngle_) : pros::Vision(port), cameraHeight(cameraHeight_), depressionAngle(depressionAngle_)
{
  set_exposure(exposure);
  set_signature(RED, &redsig);
  set_signature(BLUE, &bluesig);
  set_zero_point(pros::vision_zero_e_t::E_VISION_ZERO_CENTER);
  minBallArea = 60;
  maxBallArea = 220; // very lenient values (this is mostly to filter out the little blips the camera catches that definitely aren't Balls)
  ballRadius = 8;
  maxMotionToleranceSq = 16*16;
}

template<std::size_t n> void Vision<n>::update()
{
  // get array from pros vision api
  const int SIZE = 8; // max objects the robot will be able to see at once
  pros::vision_object_s_t arr[SIZE];
  int count = read_by_size(0, SIZE, arr);
  if(count > SIZE) // if it returns an error, quit
    return;

  // compute approximate projected ball areas and take note of all non-Ball objects in a parallel array
  bool withinAreaBounds[count];
  for(int i = 0; i < count; i++)
  {
    float projectedArea = getProjectedArea(arr[i]);
    if(projectedArea >= minBallArea && projectedArea <= maxBallArea)
      withinAreaBounds[i] = true;
    else
      withinAreaBounds[i] = false;
  }

  // remove any existing balls not present in current snapshot
  for(typename std::vector<Ball<n>>::iterator it = balls.begin(); it != balls.end(); )
  {
    bool match = false;
    for(int i = 0; i < count; i++)
    {
      if(withinAreaBounds[i] && arr[i].signature == it->color && pow(it->x.getOutput() - arr[i].x_middle_coord, 2) + pow(it->y.getOutput() - arr[i].y_middle_coord, 2) <= maxMotionToleranceSq) // if it's the same color and it didn't move too far since last frame
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
      for(typename std::vector<Ball<n>>::iterator it = balls.begin(); it != balls.end(); ++it)
      {
        if(arr[i].signature == it->color && pow(it->x.getOutput() - arr[i].x_middle_coord, 2) + pow(it->y.getOutput() - arr[i].y_middle_coord, 2) <= maxMotionToleranceSq) // if it's the same color and it didn't move too far since last frame
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
        balls.push_back(Ball<n>(arr[i].x_middle_coord, arr[i].y_middle_coord, arr[i].width, arr[i].height, static_cast<Color>(arr[i].signature)));
    }
  }
}

template<std::size_t n> float Vision<n>::getProjectedArea(pros::vision_object_s_t &obj)
{
  return powf((VISION_FOV_WIDTH_ANGLE/VISION_FOV_WIDTH)*(cameraHeight - ballRadius)/sinf(depressionAngle - (VISION_FOV_WIDTH_ANGLE/VISION_FOV_WIDTH)*obj.y_middle_coord), 2) * obj.width * obj.height;
}

template<std::size_t n> void Vision<n>::print()
{
  std::cout << "balls array size " << balls.size() << " with contents:" << std::endl;
  for(typename std::vector<Ball<n>>::iterator it = balls.begin(); it != balls.end(); ++it)
  {
    if(it->color == RED)
      std::cout << "red ";
    else if(it->color == BLUE)
      std::cout << "blue ";
    std::cout << "ball at " << it->x.getOutput() << "," << it->y.getOutput() << std::endl;
  }
}
