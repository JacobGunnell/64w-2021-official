#include "Auton.h"

void AutonBase::execWithBlackbox(Position p)
{
  Gary->clearBlackbox();
  pros::Task blackboxLoggerTask(blackboxLoggerCb);
  exec(p);
  blackboxLoggerTask.remove();
  if(Gary->saveBlackbox())
    std::cout << "Successfully saved blackbox data" << std::endl;
  else
    std::cout << "Failed to save blackbox data!" << std::endl;
}

std::vector<std::unique_ptr<AutonBase>> AutonBase::getAllObjs()
{
  std::vector<std::unique_ptr<AutonBase>> objs;
  // (Auton.h > ) ...and add one of these
  objs.push_back(std::make_unique<Illuminati>());
  objs.push_back(std::make_unique<Skills14789>());
  objs.push_back(std::make_unique<Skills147>());
  objs.push_back(std::make_unique<Skills159>());
  objs.push_back(std::make_unique<Skills14>());
  //objs.push_back(std::unique_ptr<AutonBase>(new HomeRow));

  objs.push_back(std::make_unique<NoAuton>()); // leave this one last
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

void AutonBase::initAll()
{
  std::vector<std::unique_ptr<AutonBase>> objs = getAllObjs();
  for(const auto& o : objs)
    o->init();
}

void AutonBase::blackboxLoggerCb()
{
  Rate r;
  while(true)
  {
    Gary->logBlackboxFrame();
    r.delay(10_Hz);
  }
}
