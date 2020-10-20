#include "Auton.h"

std::vector<std::unique_ptr<AutonBase>> AutonBase::getAllObjs()
{
  std::vector<std::unique_ptr<AutonBase>> objs;
  // (Auton.h > ) ...and add one of these
  objs.push_back(std::unique_ptr<AutonBase>(new HomeRow));

  objs.push_back(std::unique_ptr<AutonBase>(new NoAuton)); // leave this one last
  return objs;
}

std::string AutonBase::getAllListings()
{
  std::vector<std::unique_ptr<AutonBase>> objs = getAllObjs();
  std::string list;
  for(const auto& o : objs)
    list.append(o->getListing() + '\n');
  if(!list.empty())
    list.pop_back();
  return list;
}
