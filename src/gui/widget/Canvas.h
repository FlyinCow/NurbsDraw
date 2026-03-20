#pragma once

#include "imgui.h"
#include "math/Vector.h"
#include <functional>
#include <vector>

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

// Callback-based canvas for rendering arbitrary curves
// Evaluator: function that takes parameter t and returns Vector on curve
using CurveEvaluator = std::function<Vector(double)>;

// Extended canvas with custom curve rendering
// ctrl_points: control points for editing
// evaluator: optional function to evaluate complete curve points
// preview_evaluator: optional function to evaluate preview curve (when complete curve not available)
// interpolate_degree: if > 0, use interpolation mode with this degree
// drawing/dragging_idx/view_offset/view_scale: same as above
void draw_extended_canvas(std::vector<Vector> &ctrl_points,
                          bool &drawing,
                          int &dragging_idx,
                          ImVec2 &view_offset,
                          float &view_scale,
                          CurveEvaluator evaluator = nullptr,
                          CurveEvaluator preview_evaluator = nullptr,
                          int interpolate_degree = 0);
