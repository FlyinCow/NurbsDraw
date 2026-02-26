#include "DrawBezier.h"
#include "Manager.h"
#include "imgui.h"
#include "../widget/Canvas.h"
#include "../widget/CoordList.h"
#include "../widget/ControlButton.h"
#include <string>

void draw_bezier(WindowManager &wm, std::string window_name) {
    // persistent state for this window (can be extended for multiple instances)
    static std::vector<Vector> ctrl_points;
    static bool drawing = false;
    static int dragging_index = -1;
    static ImVec2 view_offset = ImVec2(0, 0);
    static float view_scale = 1.0f;

    ImGui::Begin(window_name.c_str());
    // three-column layout: button, canvas, coordinates
    ImGui::Columns(3, nullptr, true);

    // control toggle button
    if (draw_control_button(drawing)) {
        if (drawing) {
            // new curve started
            ctrl_points.clear();
            dragging_index = -1;
        } else {
            // stopped
            dragging_index = -1;
        }
    }
    ImGui::NextColumn();

    // main interactive canvas widget
    draw_curve_canvas(ctrl_points, drawing, dragging_index, view_offset, view_scale);
    ImGui::NextColumn();

    // coordinate listing widget
    draw_coordinate_list(ctrl_points);

    ImGui::Columns(1);
    ImGui::End();
}