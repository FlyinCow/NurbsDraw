#include "AppController.h"
#include <variant>
#include <cstddef>

void AppController::undo() {
    cmd_mgr_.undo();
}

void AppController::redo() {
    cmd_mgr_.redo();
}

size_t AppController::add_curve(const std::string& name, const std::vector<Vector>& control_points, int degree) {
    size_t index = doc_.curves.size();
    doc_.add_curve(name, control_points, degree);
    return index;
}

size_t AppController::add_curve(const std::string& name, const BCurve& curve) {
    size_t index = doc_.curves.size();
    doc_.add_curve(name, curve);
    return index;
}

void AppController::remove_curve(size_t index) {
    if (index >= doc_.curves.size()) return;
    doc_.remove_curve(index);
}

void AppController::add_control_point(size_t curve_index, const Vector& point, size_t position) {
    if (curve_index >= doc_.curves.size()) return;
    auto& item = doc_.curves[curve_index];

    std::visit([&](auto&& c) {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, std::vector<Vector>>) {
            auto cmd = std::make_unique<AddPointCommand>(c, point, position);
            cmd_mgr_.execute(std::move(cmd));
        } else if constexpr (std::is_same_v<T, BCurve>) {
            // BCurve doesn't support mid-curve insertion - ignore
            (void)point;
            (void)position;
        }
    }, item.curve);
}

void AppController::add_control_point(size_t curve_index, const Vector& point) {
    if (curve_index >= doc_.curves.size()) return;
    auto& item = doc_.curves[curve_index];

    std::visit([&](auto&& c) {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, std::vector<Vector>>) {
            auto cmd = std::make_unique<AddPointCommand>(c, point, c.size());
            cmd_mgr_.execute(std::move(cmd));
        } else if constexpr (std::is_same_v<T, BCurve>) {
            // BCurve manipulation requires full knot recomputation
            // For now, rebuild the curve with new control points
            (void)point;
        }
    }, item.curve);
}

void AppController::remove_control_point(size_t curve_index, size_t point_index) {
    if (curve_index >= doc_.curves.size()) return;
    auto& item = doc_.curves[curve_index];

    std::visit([&](auto&& c) {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, std::vector<Vector>>) {
            if (point_index < c.size()) {
                auto cmd = std::make_unique<RemovePointCommand>(c, point_index);
                cmd_mgr_.execute(std::move(cmd));
            }
        } else if constexpr (std::is_same_v<T, BCurve>) {
            // BCurve remove not implemented yet
        }
    }, item.curve);
}

void AppController::move_control_point(size_t curve_index, size_t point_index, const Vector& new_position) {
    if (curve_index >= doc_.curves.size()) return;
    auto& item = doc_.curves[curve_index];

    std::visit([&](auto&& c) {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, std::vector<Vector>>) {
            if (point_index < c.size()) {
                auto cmd = std::make_unique<MovePointCommand>(c, point_index, new_position);
                cmd_mgr_.execute(std::move(cmd));
            }
        } else if constexpr (std::is_same_v<T, BCurve>) {
            // BCurve manipulation requires rebuilding with new control points
            auto pts = c.get_control_points();
            if (point_index < pts.size()) {
                pts[point_index] = new_position;
                c.set_control_points(pts);
            }
        }
    }, item.curve);
}

void AppController::clear_all() {
    auto cmd = std::make_unique<BatchCommand>(
        [this]() { doc_.clear(); },
        []() { /* undo would need to store all curves - TODO */ },
        "Clear All"
    );
    cmd_mgr_.execute(std::move(cmd));
}