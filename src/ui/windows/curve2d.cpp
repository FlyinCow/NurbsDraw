#include "curve2d.h"
#include "../canvas.h"
#include "imgui.h"
#include "math/BCurve.h"
#include <vector>
#include <memory>
#include <string>
#include <format>

namespace {
// Curve editing state
struct Curve2DState {
    bool drawing = false;
    int dragging_idx = -1;
    ImVec2 view_offset{-100.0f, -100.0f};
    float view_scale = 5.0f;

    // Curve type: 0 = Bezier, 1 = B-spline
    int curve_type = 1;
    int degree = 3;
    bool is_nurbs = false;
    std::vector<double> weights;

    // Drawing mode: 0 = by control points, 1 = by interpolation
    int draw_mode = 0;

    // Cached curve for rendering
    std::shared_ptr<BCurve> cached_curve;
    std::vector<Vector> cached_ctrl_points;
    int cached_degree = -1;
};

static Curve2DState g_state;

// Static control points - preserved between calls
// TODO: Integrate properly with AppController for undo/redo support
static std::vector<Vector> g_ctrl_points;
}  // namespace

// Simple control button
static bool draw_control_button(bool& drawing) {
    if (ImGui::Button(drawing ? "Stop" : "Draw")) {
        drawing = !drawing;
        return true;
    }
    return false;
}

// Coordinate list widget
static void draw_coordinate_list(const std::vector<Vector>& ctrl_points) {
    ImGui::BeginChild("coords", ImVec2(0, 0), false);
    ImGui::Text("Points: %d", (int)ctrl_points.size());
    for (int i = 0; i < (int)ctrl_points.size(); ++i) {
        ImGui::Text("%d: %.1f, %.1f", i, ctrl_points[i].x(), ctrl_points[i].y());
    }
    ImGui::EndChild();
}

void draw_curve2d_window(AppController& app) {
    auto& state = g_state;
    auto& ctrl_points = g_ctrl_points;  // Use static storage

    // Update weights when NURBS mode changes
    if (state.is_nurbs && state.weights.size() != ctrl_points.size()) {
        state.weights.resize(ctrl_points.size(), 1.0);
    }

    // Rebuild curve cache if needed
    bool need_rebuild = state.cached_degree != state.degree ||
                        state.cached_ctrl_points.size() != ctrl_points.size();
    if (!need_rebuild && !state.cached_ctrl_points.empty()) {
        for (size_t i = 0; i < ctrl_points.size(); i++) {
            if (state.cached_ctrl_points[i].x() != ctrl_points[i].x() ||
                state.cached_ctrl_points[i].y() != ctrl_points[i].y() ||
                state.cached_ctrl_points[i].z() != ctrl_points[i].z()) {
                need_rebuild = true;
                break;
            }
        }
    }

    if (need_rebuild && ctrl_points.size() >= static_cast<size_t>(state.degree + 1)) {
        int n = static_cast<int>(ctrl_points.size()) - 1;
        int num_unique = n - state.degree + 2;
        if (num_unique < 2) num_unique = 2;

        std::vector<double> unique_knots(num_unique);
        std::vector<int> mults(num_unique);
        for (int i = 0; i < num_unique; i++) {
            unique_knots[i] = static_cast<double>(i) / (num_unique - 1);
        }
        mults[0] = state.degree + 1;
        mults[num_unique - 1] = state.degree + 1;
        for (int i = 1; i < num_unique - 1; i++) {
            mults[i] = 1;
        }

        if (state.is_nurbs && !state.weights.empty()) {
            state.cached_curve = std::make_shared<BCurve>(state.degree, ctrl_points, state.weights, unique_knots, mults);
        } else {
            state.cached_curve = std::make_shared<BCurve>(state.degree, ctrl_points, unique_knots, mults);
        }

        state.cached_ctrl_points = ctrl_points;
        state.cached_degree = state.degree;
    }

    ImGui::Begin("2D Curves");

    // Three-column layout
    ImGui::Columns(3, nullptr, true);

    // Column 1: Controls
    if (draw_control_button(state.drawing)) {
        if (state.drawing) {
            // Started drawing - clear points
            ctrl_points.clear();
            state.cached_curve.reset();
            state.cached_ctrl_points.clear();
            state.dragging_idx = -1;
            if (state.is_nurbs) state.weights.clear();
        } else {
            // Stopped drawing - finalize
            state.dragging_idx = -1;
        }
    }

    ImGui::Separator();
    ImGui::Text("Curve Type:");
    ImGui::RadioButton("Bezier", &state.curve_type, 0);
    ImGui::RadioButton("B-spline", &state.curve_type, 1);

    ImGui::Separator();
    ImGui::Text("Degree:");
    ImGui::SetNextItemWidth(60);
    ImGui::SliderInt("##degree", &state.degree, 1, 10);

    if (state.curve_type == 1) {
        ImGui::Checkbox("NURBS", &state.is_nurbs);
    }

    ImGui::NextColumn();

    // Column 2: Canvas
    CanvasState canvas_state;
    canvas_state.drawing = state.drawing;
    canvas_state.dragging_idx = state.dragging_idx;
    canvas_state.view_offset = state.view_offset;
    canvas_state.view_scale = state.view_scale;

    auto curve_evaluator = [&](double t) -> Vector {
        if (!state.cached_curve || ctrl_points.size() < static_cast<size_t>(state.degree + 1)) {
            return Vector{};
        }
        return state.cached_curve->eval(t);
    };

    auto preview_evaluator = [&](double t) -> Vector {
        if (ctrl_points.size() < 2) return Vector{};

        if (state.draw_mode == 0) {
            int preview_degree = std::min(state.degree, static_cast<int>(ctrl_points.size()) - 1);
            if (preview_degree < 1) preview_degree = 1;

            int n = static_cast<int>(ctrl_points.size()) - 1;
            int num_unique = n - preview_degree + 2;
            if (num_unique < 2) num_unique = 2;

            std::vector<double> unique_knots(num_unique);
            std::vector<int> mults(num_unique);
            for (int i = 0; i < num_unique; i++) {
                unique_knots[i] = static_cast<double>(i) / (num_unique - 1);
            }
            mults[0] = preview_degree + 1;
            mults[num_unique - 1] = preview_degree + 1;
            for (int i = 1; i < num_unique - 1; i++) {
                mults[i] = 1;
            }

            BCurve preview_curve(preview_degree, ctrl_points, unique_knots, mults);
            return preview_curve.eval(t);
        }
        return Vector{};
    };

    draw_canvas(canvas_state, ctrl_points, curve_evaluator, preview_evaluator,
                state.draw_mode == 1 ? state.degree : 0);

    // Sync state back
    state.drawing = canvas_state.drawing;
    state.dragging_idx = canvas_state.dragging_idx;
    state.view_offset = canvas_state.view_offset;
    state.view_scale = canvas_state.view_scale;

    ImGui::NextColumn();

    // Column 3: Coordinates
    draw_coordinate_list(ctrl_points);

    ImGui::Columns(1);
    ImGui::End();
}