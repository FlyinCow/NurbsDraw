#pragma once

#include "math/Vector.h"
#include <vector>
#include "imgui.h"

// Draws the interactive canvas.  All state is passed by reference so the
// caller retains ownership.
//
// ctrl_points : list of current control points in world coordinates.
// drawing     : whether we're currently capturing points (Draw button active)
// dragging_idx : index of control point being dragged, or -1
// view_offset  : world coordinate of top-left corner of canvas
// view_scale   : pixels-per-world-unit scale
void draw_curve_canvas(std::vector<Vector> &ctrl_points,
                       bool &drawing,
                       int &dragging_idx,
                       ImVec2 &view_offset,
                       float &view_scale);
