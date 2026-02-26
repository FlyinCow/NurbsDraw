#include "ControlButton.h"
#include "imgui.h"

bool draw_control_button(bool &drawing) {
    const char *label = drawing ? "Stop" : "Draw";
    if (ImGui::Button(label)) {
        drawing = !drawing;
        return true;
    }
    return false;
}
