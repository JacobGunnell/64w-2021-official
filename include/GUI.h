#ifndef GUI_H
#define GUI_H


#include "pros/apix.h"
#include <string>
#include "Auton.h"

void gui_loading_start();
void gui_loading_stop();
void gui_error(std::string);
void gui_main();

#endif // GUI_H
