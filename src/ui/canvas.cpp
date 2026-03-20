#include "canvas.h"
#include "math/BezierCurve.h"
#include "math/BCurve.h"
#include <cmath>
#include <string>
#include <algorithm>

void draw_canvas(CanvasState& state,
                 std::vector<Vector>& ctrl_points,
                 CurveEvaluator evaluator,
                 CurveEvaluator preview_evaluator,
                 int interpolate_degree) {
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();

    // Guard against zero or negative size
    if (canvas_sz.x <= 0 || canvas_sz.y <= 0) {
        return;
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    bool use_preview = state.drawing && (preview_evaluator || interpolate_degree > 0);

    auto world_to_screen = [&](ImVec2 w) {
        return ImVec2(canvas_p0.x + (w.x - state.view_offset.x) * state.view_scale,
                      canvas_p0.y + (w.y - state.view_offset.y) * state.view_scale);
    };
    auto screen_to_world = [&](ImVec2 s) {
        return ImVec2((s.x - canvas_p0.x) / state.view_scale + state.view_offset.x,
                      (s.y - canvas_p0.y) / state.view_scale + state.view_offset.y);
    };

    // Draw canvas border
    draw_list->AddRect(canvas_p0,
                       ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y),
                       IM_COL32(255, 255, 255, 255));

    ImGui::InvisibleButton("canvas", canvas_sz,
                           ImGuiButtonFlags_MouseButtonLeft);
    ImVec2 mouse_screen = ImGui::GetIO().MousePos;
    bool hovered = ImGui::IsItemHovered();

    // Pan/zoom
    if (hovered) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            ImVec2 before = screen_to_world(mouse_screen);
            float factor = powf(1.1f, wheel);
            state.view_scale = state.view_scale * factor;
            if (state.view_scale < 0.1f) state.view_scale = 0.1f;
            if (state.view_scale > 100.0f) state.view_scale = 100.0f;
            ImVec2 after = screen_to_world(mouse_screen);
            state.view_offset.x += before.x - after.x;
            state.view_offset.y += before.y - after.y;
        }
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            state.view_offset.x -= delta.x / state.view_scale;
            state.view_offset.y -= delta.y / state.view_scale;
        }
    }

    // Handle adding/dragging points
    if (hovered) {
        if (state.drawing) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                ImVec2 w = screen_to_world(mouse_screen);
                ctrl_points.emplace_back(w.x, w.y, 0.0);
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                state.drawing = false;
            }
        } else {
            if (state.dragging_idx == -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                const float radius = 6.0f;
                for (int i = 0; i < (int)ctrl_points.size(); ++i) {
                    ImVec2 screen = world_to_screen(ImVec2(ctrl_points[i].x(), ctrl_points[i].y()));
                    float dx = ImGui::GetIO().MousePos.x - screen.x;
                    float dy = ImGui::GetIO().MousePos.y - screen.y;
                    if (dx * dx + dy * dy < radius * radius) {
                        state.dragging_idx = i;
                        break;
                    }
                }
            }
            if (state.dragging_idx != -1 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                ImVec2 w = screen_to_world(mouse_screen);
                ctrl_points[state.dragging_idx] = Vector(w.x, w.y, 0.0);
            }
            if (state.dragging_idx != -1 && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                state.dragging_idx = -1;
            }
        }
    }

    // Evaluation points for drawing (including preview)
    std::vector<Vector> eval_points = ctrl_points;
    if (state.drawing && !ctrl_points.empty()) {
        ImVec2 w = screen_to_world(mouse_screen);
        eval_points.emplace_back(w.x, w.y, 0.0);
    }

    // Draw control polygon
    for (int i = 0; i < (int)ctrl_points.size(); ++i) {
        ImVec2 p = world_to_screen(ImVec2(ctrl_points[i].x(), ctrl_points[i].y()));
        draw_list->AddCircleFilled(p, 4.0f, IM_COL32(255, 0, 0, 255));
        if (i > 0) {
            ImVec2 prev = world_to_screen(ImVec2(ctrl_points[i - 1].x(), ctrl_points[i - 1].y()));
            draw_list->AddLine(prev, p, IM_COL32(0, 255, 0, 255), 1.0f);
        }
    }

    // Draw preview point (mouse position) when drawing
    if (state.drawing && hovered) {
        ImVec2 w = screen_to_world(mouse_screen);
        ImVec2 screen_pt = world_to_screen(w);
        draw_list->AddCircleFilled(screen_pt, 5.0f, IM_COL32(255, 255, 0, 200));
    }

    // Show hint message when drawing mode is active
    if (state.drawing) {
        int required_pts = interpolate_degree > 0 ? (interpolate_degree + 1) : 4;

        std::string hint_msg;
        if (interpolate_degree > 0) {
            if (eval_points.size() < static_cast<size_t>(interpolate_degree + 1)) {
                hint_msg = "Need " + std::to_string(interpolate_degree + 1) + " pts for degree " + std::to_string(interpolate_degree) + " interpolate";
            }
        } else {
            if (ctrl_points.size() < 2) {
                hint_msg = "Click to add control points";
            } else if (ctrl_points.size() < 4) {
                hint_msg = "Add more points (need " + std::to_string(4 - ctrl_points.size()) + " more for degree 3)";
            }
        }

        if (!hint_msg.empty()) {
            ImVec2 hint_pos(canvas_p0.x + 10, canvas_p0.y + canvas_sz.y - 30);
            draw_list->AddRectFilled(ImVec2(hint_pos.x - 5, hint_pos.y - 5),
                                     ImVec2(hint_pos.x + 280, hint_pos.y + 20),
                                     IM_COL32(0, 0, 0, 180));
            draw_list->AddText(hint_pos, IM_COL32(255, 255, 255, 255), hint_msg.c_str());
        }
    }

    // Draw curve
    CurveEvaluator active_evaluator = evaluator;
    if (use_preview && preview_evaluator) {
        active_evaluator = preview_evaluator;
    }

    if (use_preview && eval_points.size() >= 2) {
        const int segments = 100;
        std::shared_ptr<BCurve> preview_curve;
        ImU32 curve_color = IM_COL32(255, 255, 0, 200);

        if (interpolate_degree > 0 && eval_points.size() >= static_cast<size_t>(interpolate_degree + 1)) {
            int n = static_cast<int>(eval_points.size());

            for (int seg = 0; seg < n - 1; seg++) {
                Vector p0, p1, p2, p3;

                if (seg == 0) {
                    p0 = eval_points[0];
                    p1 = eval_points[0];
                    p2 = eval_points[1];
                    p3 = eval_points[2 % n];
                } else if (seg == n - 2) {
                    p0 = eval_points[seg - 1];
                    p1 = eval_points[seg];
                    p2 = eval_points[seg + 1];
                    p3 = eval_points[seg + 1];
                } else {
                    p0 = eval_points[seg - 1];
                    p1 = eval_points[seg];
                    p2 = eval_points[seg + 1];
                    p3 = eval_points[seg + 2];
                }

                int segs_per_span = segments / (n - 1);

                for (int i = 1; i <= segs_per_span; i++) {
                    double t = static_cast<double>(i) / segs_per_span;

                    double t2 = t * t;
                    double t3 = t2 * t;

                    double b0 = -0.5 * t3 + t2 - 0.5 * t;
                    double b1 = 1.5 * t3 - 2.5 * t2 + 1.0;
                    double b2 = -1.5 * t3 + 2.0 * t2 + 0.5 * t;
                    double b3 = 0.5 * t3 - 0.5 * t2;

                    Vector p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;

                    double prev_t = static_cast<double>(i - 1) / segs_per_span;
                    double prev_t2 = prev_t * prev_t;
                    double prev_t3 = prev_t2 * prev_t;

                    double pb0 = -0.5 * prev_t3 + prev_t2 - 0.5 * prev_t;
                    double pb1 = 1.5 * prev_t3 - 2.5 * prev_t2 + 1.0;
                    double pb2 = -1.5 * prev_t3 + 2.0 * prev_t2 + 0.5 * prev_t;
                    double pb3 = 0.5 * prev_t3 - 0.5 * prev_t2;

                    Vector prev_p = p0 * pb0 + p1 * pb1 + p2 * pb2 + p3 * pb3;

                    ImVec2 p1_screen = world_to_screen(ImVec2(prev_p.x(), prev_p.y()));
                    ImVec2 p2_screen = world_to_screen(ImVec2(p.x(), p.y()));
                    draw_list->AddLine(p1_screen, p2_screen, curve_color, 2.0f);
                }
            }
        } else {
            int preview_degree;
            if (eval_points.size() >= static_cast<size_t>(interpolate_degree > 0 ? interpolate_degree + 1 : 4)) {
                preview_degree = std::min(3, static_cast<int>(eval_points.size()) - 1);
            } else {
                preview_degree = std::min(3, static_cast<int>(eval_points.size()) - 1);
            }
            if (preview_degree < 1) preview_degree = 1;

            int n = static_cast<int>(eval_points.size()) - 1;
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

            preview_curve = std::make_shared<BCurve>(preview_degree, eval_points, unique_knots, mults);
        }

        if (preview_curve) {
            for (int i = 1; i <= segments; ++i) {
                double t = static_cast<double>(i) / segments;
                Vector p = preview_curve->eval(t);
                Vector prev_p = preview_curve->eval(static_cast<double>(i - 1) / segments);

                ImVec2 p1_screen = world_to_screen(ImVec2(prev_p.x(), prev_p.y()));
                ImVec2 p2_screen = world_to_screen(ImVec2(p.x(), p.y()));
                draw_list->AddLine(p1_screen, p2_screen, curve_color, 2.0f);
            }
        }
    } else if (eval_points.size() >= 2 && evaluator) {
        const int segments = 100;
        ImU32 curve_color = IM_COL32(255, 255, 255, 255);
        for (int i = 1; i <= segments; ++i) {
            double t = static_cast<double>(i) / segments;
            Vector p = evaluator(t);
            Vector prev_p = evaluator(static_cast<double>(i - 1) / segments);
            ImVec2 p1 = world_to_screen(ImVec2(prev_p.x(), prev_p.y()));
            ImVec2 p2 = world_to_screen(ImVec2(p.x(), p.y()));
            draw_list->AddLine(p1, p2, curve_color, 2.0f);
        }
    }

    // Draw axes (clamped to canvas bounds)
    ImVec2 origin_screen = world_to_screen(ImVec2(0, 0));
    ImVec2 canvas_p1(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // X-axis: horizontal line at origin_screen.y, clamped to canvas width
    float xAxisY = std::clamp(origin_screen.y, canvas_p0.y, canvas_p1.y);
    if (origin_screen.y >= canvas_p0.y && origin_screen.y <= canvas_p1.y) {
        draw_list->AddLine(ImVec2(canvas_p0.x, xAxisY), ImVec2(canvas_p1.x, xAxisY), IM_COL32(200, 200, 200, 255));
    }

    // Y-axis: vertical line at origin_screen.x, clamped to canvas height
    float yAxisX = std::clamp(origin_screen.x, canvas_p0.x, canvas_p1.x);
    if (origin_screen.x >= canvas_p0.x && origin_screen.x <= canvas_p1.x) {
        draw_list->AddLine(ImVec2(yAxisX, canvas_p0.y), ImVec2(yAxisX, canvas_p1.y), IM_COL32(200, 200, 200, 255));
    }

    // Origin dot (only if within canvas)
    if (origin_screen.x >= canvas_p0.x && origin_screen.x <= canvas_p1.x &&
        origin_screen.y >= canvas_p0.y && origin_screen.y <= canvas_p1.y) {
        draw_list->AddCircleFilled(origin_screen, 5.0f, IM_COL32(255, 255, 0, 255));
    }

    float min_x = state.view_offset.x;
    float max_x = state.view_offset.x + canvas_sz.x / state.view_scale;
    float min_y = state.view_offset.y;
    float max_y = state.view_offset.y + canvas_sz.y / state.view_scale;

    float target_pixel_spacing = 80.0f;
    float tick_interval = std::pow(10.0f, std::floor(std::log10(target_pixel_spacing / state.view_scale) + 0.5f));
    if (tick_interval < 1.0f) tick_interval = 1.0f;

    int start_x = (int)std::floor(min_x / tick_interval);
    int end_x = (int)std::ceil(max_x / tick_interval);
    float xAxisYtop = std::clamp(origin_screen.y - 5, canvas_p0.y, canvas_p1.y);
    float xAxisYbot = std::clamp(origin_screen.y + 5, canvas_p0.y, canvas_p1.y);
    for (int xi = start_x; xi <= end_x; ++xi) {
        float world_x = xi * tick_interval;
        ImVec2 pt = world_to_screen(ImVec2(world_x, 0.0f));
        float x = pt.x;
        if (x >= canvas_p0.x && x <= canvas_p0.x + canvas_sz.x) {
            draw_list->AddLine(ImVec2(x, xAxisYtop), ImVec2(x, xAxisYbot), IM_COL32(180, 180, 180, 255));
            std::string label = std::to_string(static_cast<int>(world_x));
            draw_list->AddText(ImVec2(x + 2, xAxisY + 2), IM_COL32(180, 180, 180, 255), label.c_str());
        }
    }

    int start_y = (int)std::floor(min_y / tick_interval);
    int end_y = (int)std::ceil(max_y / tick_interval);
    float yAxisXleft = std::clamp(origin_screen.x - 5, canvas_p0.x, canvas_p1.x);
    float yAxisXright = std::clamp(origin_screen.x + 5, canvas_p0.x, canvas_p1.x);
    for (int yi = start_y; yi <= end_y; ++yi) {
        float world_y = yi * tick_interval;
        ImVec2 pt = world_to_screen(ImVec2(0.0f, world_y));
        float y = pt.y;
        if (y >= canvas_p0.y && y <= canvas_p0.y + canvas_sz.y) {
            draw_list->AddLine(ImVec2(yAxisXleft, y), ImVec2(yAxisXright, y), IM_COL32(180, 180, 180, 255));
            std::string label = std::to_string(static_cast<int>(world_y));
            draw_list->AddText(ImVec2(yAxisX + 2, y + 2), IM_COL32(180, 180, 180, 255), label.c_str());
        }
    }
}