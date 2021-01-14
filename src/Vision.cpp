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
  int count = get_object_count();
  pros::vision_object_s_t *const arr = new pros::vision_object_s_t[count];
  read_by_size(0, count, arr);

  // remove any existing balls not present in current snapshot
  for(typename std::vector<Ball<n>>::iterator it = balls.begin(); it != balls.end(); ++it)
  {
    bool match = false;
    for(int i = 0; i < count; i++)
    {
      if(arr[i].signature == it->color && pow(it->x.getOutput() - arr[i].x_middle_coord, 2) + pow(it->y.getOutput() - arr[i].y_middle_coord, 2) <= maxFrameToleranceSq) // if it's the same color and it didn't move too far since last frame
      {
        match = true;
        break;
      }
    }
    if(!match)
      balls.erase(it);
  }

  // add any new balls, update existing
  for(int i = 0; i < count; i++)
  {
    if(arr[i].width*arr[i].height >= minBallArea)
    {
      bool match = false;
      for(Ball<n> ball : balls)
      {
        if(arr[i].signature == ball.color && pow(ball.x.getOutput() - arr[i].x_middle_coord, 2) + pow(ball.y.getOutput() - arr[i].y_middle_coord, 2) <= maxFrameToleranceSq) // if it's the same color and it didn't move too far since last frame
        {
          ball.x.filter(arr[i].x_middle_coord);
          ball.y.filter(arr[i].y_middle_coord);
          match = true;
          break;
        }
      }
      if(!match) // if no match was found, create a new object in balls
        balls.push_back(Ball<n>(arr[i].x_middle_coord, arr[i].y_middle_coord, arr[i].signature));
    }
  }
}
