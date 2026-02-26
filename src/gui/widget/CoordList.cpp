#include "CoordList.h"
#include "imgui.h"

void draw_coordinate_list(const std::vector<Vector> &ctrl_points) {
    ImGui::BeginChild("coords", ImVec2(0,0), false);
    ImGui::Text("Points: %d", (int)ctrl_points.size());
    for (int i = 0; i < (int)ctrl_points.size(); ++i) {
        ImGui::Text("%d: %.1f, %.1f", i, ctrl_points[i].x(), ctrl_points[i].y());
    }
    ImGui::EndChild();
}
