#include "DrawBSpline.h"
#include "Manager.h"
#include "imgui.h"
#include "../widget/Canvas.h"
#include "../widget/CoordList.h"
#include "../widget/ControlButton.h"
#include "math/BCurve.h"
#include <string>
#include <vector>
#include <cstdio>
#include <memory>

void draw_bspline(WindowManager &wm, std::string window_name) {
    // Persistent state for B-spline editing
    static std::vector<Vector> ctrl_points;
    static bool drawing = false;
    static int dragging_index = -1;
    static ImVec2 view_offset = ImVec2(-100.0f, -100.0f);  // Start with a reasonable offset
    static float view_scale = 5.0f;  // Start with a reasonable scale (pixels per world unit)
    
    // B-spline parameters
    static int degree = 3;
    static bool is_nurbs = false;
    static std::vector<double> weights;
    
    // Drawing mode: 0 = by control points, 1 = by interpolation
    static int draw_mode = 0;
    static const char* draw_mode_names[] = {"Draw by Ctrl Pt", "Draw by Interpolate"};
    
    // Cached curve - rebuilt when control points or degree change
    static std::shared_ptr<BCurve> cached_curve;
    static std::vector<Vector> cached_ctrl_points;
    static int cached_degree = -1;
    
    // Update weights when control points change
    if (is_nurbs && weights.size() != ctrl_points.size()) {
        weights.resize(ctrl_points.size(), 1.0);
    }
    
    // Rebuild curve if needed (when control points or degree change)
    bool need_rebuild = cached_degree != degree || 
                        cached_ctrl_points.size() != ctrl_points.size();
    if (!need_rebuild && !cached_ctrl_points.empty()) {
        for (size_t i = 0; i < ctrl_points.size(); i++) {
            if (cached_ctrl_points[i].x() != ctrl_points[i].x() ||
                cached_ctrl_points[i].y() != ctrl_points[i].y() ||
                cached_ctrl_points[i].z() != ctrl_points[i].z()) {
                need_rebuild = true;
                break;
            }
        }
    }
    
    if (need_rebuild && ctrl_points.size() >= static_cast<size_t>(degree + 1)) {
        // For clamped B-spline: generate proper clamped knot vector
        // Number of control points = m = n + 1, degree = p
        // Number of flat knots = n + p + 2
        int n = static_cast<int>(ctrl_points.size()) - 1;
        int p = degree;
        
        // Number of unique knots = n - p + 2 (from B-spline theory)
        // Must be at least 2 for valid knot vector
        int num_unique = n - p + 2;
        if (num_unique < 2) num_unique = 2;
        
        // Generate unique knots: uniform from 0 to 1
        std::vector<double> unique_knots(num_unique);
        for (int i = 0; i < num_unique; i++) {
            unique_knots[i] = static_cast<double>(i) / (num_unique - 1);
        }
        
        // Multiplicity: p+1 at start and end, 1 in middle
        std::vector<int> mults(num_unique);
        mults[0] = p + 1;
        mults[num_unique - 1] = p + 1;
        for (int i = 1; i < num_unique - 1; i++) {
            mults[i] = 1;
        }
        
        if (is_nurbs && !weights.empty()) {
            cached_curve = std::make_shared<BCurve>(degree, ctrl_points, weights, unique_knots, mults);
        } else {
            cached_curve = std::make_shared<BCurve>(degree, ctrl_points, unique_knots, mults);
        }
        
        cached_ctrl_points = ctrl_points;
        cached_degree = degree;
    }
    
    ImGui::Begin(window_name.c_str());
    
    // Controls panel
    ImGui::Columns(3, nullptr, true);
    
    // Control toggle button
    if (draw_control_button(drawing)) {
        if (drawing) {
            ctrl_points.clear();
            cached_curve.reset();
            cached_ctrl_points.clear();
            dragging_index = -1;
            if (is_nurbs) weights.clear();
        } else {
            dragging_index = -1;
        }
    }
    ImGui::NextColumn();
    
    // B-spline parameter controls
    ImGui::Text("Mode:"); 
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    if (ImGui::Combo("##drawmode", &draw_mode, draw_mode_names, 2)) {
        // Clear points when mode changes
        ctrl_points.clear();
        cached_curve.reset();
        cached_ctrl_points.clear();
    }
    
    ImGui::Text("Degree:"); 
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    ImGui::SliderInt("##degree", &degree, 1, 10);
    
    ImGui::Text("NURBS:"); 
    ImGui::SameLine();
    ImGui::Checkbox("##nurbs", &is_nurbs);
    
    // Create curve evaluator callback - uses cached curve
    auto curve_evaluator = [&](double t) -> Vector {
        if (!cached_curve || ctrl_points.size() < static_cast<size_t>(degree + 1)) {
            return Vector{};
        }
        
        Vector result = cached_curve->eval(t);
        return result;
    };
    
    // Create preview evaluator - works even with insufficient control points
    auto preview_evaluator = [&](double t) -> Vector {
        if (ctrl_points.size() < 2) {
            return Vector{};
        }
        
        if (draw_mode == 0) {
            // Draw by Control Pt mode: use control points directly
            // If we have at least degree + 1 points, use the requested degree
            // Otherwise use a lower degree for preview
            int preview_degree;
            if (ctrl_points.size() >= static_cast<size_t>(degree + 1)) {
                preview_degree = degree;
            } else {
                preview_degree = std::min(degree, static_cast<int>(ctrl_points.size()) - 1);
            }
            if (preview_degree < 1) preview_degree = 1;
            
            // Generate knots for preview
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
        } else {
            // Draw by Interpolate mode: interpolate through sample points
            // This needs eval_points from canvas, so we handle it differently
            return Vector{};
        }
    };
    
    // Canvas with curve rendering
    int interp_degree = (draw_mode == 1) ? degree : 0;
    draw_extended_canvas(ctrl_points, drawing, dragging_index, 
                        view_offset, view_scale, curve_evaluator, preview_evaluator, interp_degree);
    ImGui::NextColumn();
    
    // Coordinate listing
    draw_coordinate_list(ctrl_points);
    
    // Display knot information
    if (ctrl_points.size() >= static_cast<size_t>(degree + 1)) {
        ImGui::Separator();
        
        // Generate knots for display (same as evaluator)
        int n = static_cast<int>(ctrl_points.size()) - 1;
        int num_unique = n - degree + 3;
        if (num_unique < 2) num_unique = 2;
        
        std::vector<double> knots(num_unique);
        knots[0] = 0.0;
        knots[num_unique - 1] = 1.0;
        for (int i = 1; i < num_unique - 1; i++) {
            knots[i] = static_cast<double>(i) / (num_unique - 1);
        }
        
        ImGui::Text("Knots:");
        std::string knot_str;
        for (size_t i = 0; i < knots.size(); i++) {
            knot_str += std::format("{:.2f} ", knots[i]);
        }
        ImGui::TextWrapped("%s", knot_str.c_str());
        
        ImGui::Text("Ctrl Pts: %zu, Degree: %d", ctrl_points.size(), degree);
        
        if (is_nurbs && ImGui::CollapsingHeader("Weights")) {
            for (size_t i = 0; i < weights.size(); i++) {
                float w = static_cast<float>(weights[i]);
                ImGui::SliderFloat(std::format("##w{}", i).c_str(), &w, 0.0f, 5.0f);
                weights[i] = static_cast<double>(w);
            }
        }
    }
    
    ImGui::Columns(1);
    ImGui::End();
}
