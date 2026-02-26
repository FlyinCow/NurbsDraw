#include "Canvas.h"
#include "math/BezierCurve.h" // for sample function
#include <cmath>
#include <string>

void draw_curve_canvas(std::vector<Vector> &ctrl_points,
                       bool &drawing,
                       int &dragging_idx,
                       ImVec2 &view_offset,
                       float &view_scale) {
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();    // top-left corner
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail(); // size of canvas
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    // coordinate transforms
    auto world_to_screen = [&](ImVec2 w) {
        return ImVec2(canvas_p0.x + (w.x - view_offset.x) * view_scale,
                      canvas_p0.y + (w.y - view_offset.y) * view_scale);
    };
    auto screen_to_world = [&](ImVec2 s) {
        return ImVec2((s.x - canvas_p0.x) / view_scale + view_offset.x,
                      (s.y - canvas_p0.y) / view_scale + view_offset.y);
    };

    // draw canvas border
    draw_list->AddRect(canvas_p0,
                       ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y),
                       IM_COL32(255, 255, 255, 255));

    ImGui::InvisibleButton("canvas", canvas_sz,
                           ImGuiButtonFlags_MouseButtonLeft |
                               ImGuiButtonFlags_MouseButtonRight |
                               ImGuiButtonFlags_MouseButtonMiddle);
    ImVec2 mouse_screen = ImGui::GetIO().MousePos;
    ImVec2 mouse_pos_in_canvas = ImVec2(mouse_screen.x - canvas_p0.x,
                                        mouse_screen.y - canvas_p0.y);
    bool hovered = ImGui::IsItemHovered();

    // pan/zoom
    if (hovered) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            ImVec2 before = screen_to_world(mouse_screen);
            float factor = powf(1.1f, wheel);
            view_scale = view_scale * factor;
            if (view_scale < 0.1f)
                view_scale = 0.1f;
            if (view_scale > 100.0f)
                view_scale = 100.0f;
            ImVec2 after = screen_to_world(mouse_screen);
            view_offset.x += before.x - after.x;
            view_offset.y += before.y - after.y;
        }
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            view_offset.x -= delta.x / view_scale;
            view_offset.y -= delta.y / view_scale;
        }
    }

    // handle adding/draging points
    if (hovered) {
        if (drawing) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                ImVec2 w = screen_to_world(mouse_screen);
                ctrl_points.emplace_back(w.x, w.y, 0.0);
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                drawing = false; // caller tracks this change
            }
        } else {
            if (dragging_idx == -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                const float radius = 6.0f;
                for (int i = 0; i < (int)ctrl_points.size(); ++i) {
                    ImVec2 screen = world_to_screen(ImVec2(ctrl_points[i].x(), ctrl_points[i].y()));
                    float dx = ImGui::GetIO().MousePos.x - screen.x;
                    float dy = ImGui::GetIO().MousePos.y - screen.y;
                    if (dx * dx + dy * dy < radius * radius) {
                        dragging_idx = i;
                        break;
                    }
                }
            }
            if (dragging_idx != -1 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                ImVec2 w = screen_to_world(mouse_screen);
                ctrl_points[dragging_idx] = Vector(w.x, w.y, 0.0);
            }
            if (dragging_idx != -1 && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                dragging_idx = -1;
            }
        }
    }

    // evaluation points for drawing (including preview)
    std::vector<Vector> eval_points = ctrl_points;
    if (drawing && !ctrl_points.empty()) {
        ImVec2 w = screen_to_world(mouse_screen);
        eval_points.emplace_back(w.x, w.y, 0.0);
    }

    // draw control polygon
    for (int i = 0; i < (int)ctrl_points.size(); ++i) {
        ImVec2 p = world_to_screen(ImVec2(ctrl_points[i].x(), ctrl_points[i].y()));
        draw_list->AddCircleFilled(p, 4.0f, IM_COL32(255, 0, 0, 255));
        if (i > 0) {
            ImVec2 prev = world_to_screen(ImVec2(ctrl_points[i - 1].x(), ctrl_points[i - 1].y()));
            draw_list->AddLine(prev, p, IM_COL32(0, 255, 0, 255), 1.0f);
        }
    }

    // draw curve
    if (eval_points.size() >= 2) {
        BezierCurve curve(eval_points);
        auto samples = sample_bezier_uniform(curve, 100);
        for (int i = 1; i < (int)samples.size(); ++i) {
            ImVec2 p1 = world_to_screen(ImVec2(samples[i - 1].x(), samples[i - 1].y()));
            ImVec2 p2 = world_to_screen(ImVec2(samples[i].x(), samples[i].y()));
            draw_list->AddLine(p1, p2, IM_COL32(255, 255, 255, 255), 2.0f);
        }
    }

    // draw axes
    ImVec2 origin_screen = world_to_screen(ImVec2(0, 0));
    draw_list->AddLine(ImVec2(canvas_p0.x, origin_screen.y),
                       ImVec2(canvas_p0.x + canvas_sz.x, origin_screen.y),
                       IM_COL32(200, 200, 200, 255));
    draw_list->AddLine(ImVec2(origin_screen.x, canvas_p0.y),
                       ImVec2(origin_screen.x, canvas_p0.y + canvas_sz.y),
                       IM_COL32(200, 200, 200, 255));
    if (origin_screen.x >= canvas_p0.x && origin_screen.x <= canvas_p0.x + canvas_sz.x &&
        origin_screen.y >= canvas_p0.y && origin_screen.y <= canvas_p0.y + canvas_sz.y) {
        draw_list->AddCircleFilled(origin_screen, 5.0f, IM_COL32(255, 255, 0, 255));
    }
    float min_x = view_offset.x;
    float max_x = view_offset.x + canvas_sz.x / view_scale;
    float min_y = view_offset.y;
    float max_y = view_offset.y + canvas_sz.y / view_scale;
    int start_x = (int)floor(min_x);
    int end_x = (int)ceil(max_x);
    for (int xi = start_x; xi <= end_x; ++xi) {
        ImVec2 pt = world_to_screen(ImVec2((float)xi, 0.0f));
        float x = pt.x;
        if (x >= canvas_p0.x && x <= canvas_p0.x + canvas_sz.x) {
            draw_list->AddLine(ImVec2(x, origin_screen.y - 5), ImVec2(x, origin_screen.y + 5), IM_COL32(200, 200, 200, 255));
            draw_list->AddText(ImVec2(x + 2, origin_screen.y + 2), IM_COL32(200, 200, 200, 255), std::to_string(xi).c_str());
        }
    }
    int start_y = (int)floor(min_y);
    int end_y = (int)ceil(max_y);
    for (int yi = start_y; yi <= end_y; ++yi) {
        ImVec2 pt = world_to_screen(ImVec2(0.0f, (float)yi));
        float y = pt.y;
        if (y >= canvas_p0.y && y <= canvas_p0.y + canvas_sz.y) {
            draw_list->AddLine(ImVec2(origin_screen.x - 5, y), ImVec2(origin_screen.x + 5, y), IM_COL32(200, 200, 200, 255));
            draw_list->AddText(ImVec2(origin_screen.x + 2, y + 2), IM_COL32(200, 200, 200, 255), std::to_string(yi).c_str());
        }
    }
}
