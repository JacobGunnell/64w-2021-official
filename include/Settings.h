#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <fstream>

struct Settings
{
  double translationalExpo = .2;
  double rotationalDR = 1.0;
  double rotationalExpo = .15;
  bool calibrateImuOnStart = true;

  bool load(std::string filename = "/usd/settings.dat")
  {
    std::ifstream file(filename, std::ifstream::binary);
  	if(file)
  	{
  		file.read((char *)this, sizeof(Settings));
      if(file)
      {
        file.close();
        return true;
      }
      else
      {
        file.close();
        *this = Settings();
        return false;
      }
  	}
  }

  void save(std::string filename = "/usd/settings.dat")
  {
    std::ofstream file(filename, std::ofstream::binary | std::ofstream::trunc);
    file.write((char *)this, sizeof(Settings));
    file.close();
  }
};

#endif // SETTINGS_H
