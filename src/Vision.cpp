#include "Vision.h"

template<std::size_t n> Vision<n>::Vision(int port, int exposure, pros::vision_signature_s_t redsig, pros::vision_signature_s_t bluesig) : pros::Vision(port)
{
  set_exposure(exposure);
  set_signature(RED, &redsig);
  set_signature(BLUE, &bluesig);
  set_zero_point(pros::vision_zero_e_t::E_VISION_ZERO_CENTER);
  minBallArea = 0; // TODO: change
  maxFrameToleranceSq = 16*16;
}

template<std::size_t n> void Vision<n>::update()
{
  // get array from pros vision api
  const int SIZE = 8; // max objects the robot will be able to see at once
  pros::vision_object_s_t arr[SIZE];
  int count = read_by_size(0, SIZE, arr);
  if(count > SIZE) // if it returns an error, quit
    return;


  // remove any existing balls not present in current snapshot
  for(typename std::vector<Ball<n>>::iterator it = balls.begin(); it != balls.end(); )
  {
    bool match = false;
    for(int i = 0; i < count; i++)
    {
      if(arr[i].signature == it->color && pow(it->x.getOutput() - arr[i].x_middle_coord, 2) + pow(it->y.getOutput() - arr[i].y_middle_coord, 2) <= maxFrameToleranceSq && arr[i].width*arr[i].height >= minBallArea) // if it's the same color and it didn't move too far since last frame
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
    if(arr[i].width*arr[i].height >= minBallArea)
    {
      bool match = false;
      for(typename std::vector<Ball<n>>::iterator it = balls.begin(); it != balls.end(); ++it)
      {
        if(arr[i].signature == it->color && pow(it->x.getOutput() - arr[i].x_middle_coord, 2) + pow(it->y.getOutput() - arr[i].y_middle_coord, 2) <= maxFrameToleranceSq) // if it's the same color and it didn't move too far since last frame
        {
          it->x.filter(arr[i].x_middle_coord);
          it->y.filter(arr[i].y_middle_coord);
          match = true;
          break;
        }
      }
      if(!match) // if no match was found, create a new object in balls
        balls.push_back(Ball<n>(arr[i].x_middle_coord, arr[i].y_middle_coord, arr[i].signature));
    }
  }
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
