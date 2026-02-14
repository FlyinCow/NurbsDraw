#include "DrawBezier.h"
#include "Manager.h"
#include "imgui.h"
#include <string>

void DrawBezier(WindowManager &wm, std::string window_name) {
    ImGui::Begin("DrawCurve");
    auto draw_list = ImGui::GetWindowDrawList();
    ImGui::End();
}