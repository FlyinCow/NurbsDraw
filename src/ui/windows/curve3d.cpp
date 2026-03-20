#include "curve3d.h"
#include "imgui.h"

void draw_curve3d_window(AppController& app) {
    ImGui::Begin("3D Curves");

    ImGui::Text("3D Curve/Surface Editor");
    ImGui::Separator();

    // Placeholder for surface editing
    ImGui::Text("Surface editing coming soon...");
    ImGui::Text("Supported: B-spline surfaces, NURBS surfaces");

    ImGui::End();
}