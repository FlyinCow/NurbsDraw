#pragma once
#include "Command.h"
#include "Document.h"

// Application controller - single entry point for all data operations
// All UI components should interact with data through this facade
class AppController {
public:
    AppController() = default;

    // Undo/Redo
    void undo();
    void redo();
    bool can_undo() const {
        return cmd_mgr_.can_undo();
    }
    bool can_redo() const {
        return cmd_mgr_.can_redo();
    }

    // Document access
    Document &document() {
        return doc_;
    }
    const Document &document() const {
        return doc_;
    }

    // Curve operations - create new curve with control points (Bezier-style)
    size_t add_curve(const std::string &name, const std::vector<Vec3d> &control_points, int degree = 3);

    // Curve operations - create B-spline/NURBS curve
    size_t add_curve(const std::string &name, const BCurve &curve);

    // Remove curve by index
    void remove_curve(size_t index);

    // Add control point to existing curve
    void add_control_point(size_t curve_index, const Vec3d &point, size_t position);

    // Add control point at end
    void add_control_point(size_t curve_index, const Vec3d &point);

    // Remove control point from curve
    void remove_control_point(size_t curve_index, size_t point_index);

    // Move control point
    void move_control_point(size_t curve_index, size_t point_index, const Vec3d &new_position);

    // Clear all curves
    void clear_all();

private:
    Document doc_;
    CommandManager cmd_mgr_;
};