#include "ControlButton.h"
#include "imgui.h"
#include <string>

bool draw_control_button(bool &drawing) {
    return draw_control_button(drawing, "start", "stop");
}

bool draw_control_button(bool &enable, std::string enable_hint, std::string disable_hint) {
    if (ImGui::Button(enable ? disable_hint.c_str() : enable_hint.c_str())) {
        enable = !enable;
        return true;
    }
    return false;
}