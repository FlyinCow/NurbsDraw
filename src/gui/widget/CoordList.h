#pragma once

#include "math/Vector.h"
#include <vector>

// Render a simple list of control point coordinates inside the current ImGui
// window/child region.
void draw_coordinate_list(const std::vector<Vector> &ctrl_points);
