#pragma once

// Draws the draw/stop toggle button.  Returns true if the state changed.
#include <string>
bool draw_control_button(bool &drawing);

bool draw_control_button(bool &enable, std::string enable_hint, std::string disable_hint);