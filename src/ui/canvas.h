#pragma once
#include "math/Vector.h"
#include <vector>
#include <functional>
#include "imgui.h"

// Canvas state passed by reference
struct CanvasState {
    bool drawing = false;
    int dragging_idx = -1;
    ImVec2 view_offset{0, 0};
    float view_scale = 10.0f;  // pixels per world unit
};

// Callback for curve evaluation
using CurveEvaluator = std::function<Vector(double)>;

// Draw interactive canvas with control points
// state: canvas state (view transform, drag state, drawing mode)
// ctrl_points: control points to edit
// evaluator: optional function to evaluate complete curve
// preview_evaluator: optional function for preview curve while drawing
// interpolate_degree: if > 0, use interpolation mode with this degree
void draw_canvas(CanvasState& state,
                 std::vector<Vector>& ctrl_points,
                 CurveEvaluator evaluator = nullptr,
                 CurveEvaluator preview_evaluator = nullptr,
                 int interpolate_degree = 0);