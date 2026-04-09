#include "Fitting.h"
#include "BCurve.h"
#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

// Chord length parameterization - compute parameter values for data points
std::vector<double> compute_chord_length_params(const std::vector<Vec3d> &points) {
    assert(!points.empty());

    if (points.size() == 1) {
        return {0.0};
    }

    std::vector<double> params(points.size());
    params[0] = 0.0;

    double total_length = 0.0;
    for (size_t i = 1; i < points.size(); i++) {
        Vec3d diff = points[i] - points[i - 1];
        total_length += diff.norm();
        params[i] = total_length;
    }

    // Normalize to [0, 1]
    if (total_length > 0.0) {
        for (auto &p : params) {
            p /= total_length;
        }
    }

    return params;
}

// Basic least squares B-spline fitting
// This is a simplified implementation
std::vector<Vec3d> fit_bcurve_least_squares(int degree,
                                            const std::vector<Vec3d> &data_points,
                                            int num_control_points) {
    assert(degree > 0);
    assert(!data_points.empty());
    assert(num_control_points >= degree + 1);
    assert(num_control_points <= static_cast<int>(data_points.size()));

    // Use chord length parameterization
    std::vector<double> params = compute_chord_length_params(data_points);

    // Generate knots using averaging method
    std::vector<double> knots;
    knots.reserve(num_control_points + degree + 1);

    // First (degree + 1) knots are 0
    for (int i = 0; i <= degree; i++) {
        knots.push_back(0.0);
    }

    // Middle knots are averages of parameter values
    int num_internal = num_control_points - degree - 1;
    if (num_internal > 0 && params.size() > static_cast<size_t>(degree + 1)) {
        int step = static_cast<int>(params.size()) / (num_internal + 1);
        for (int i = 1; i <= num_internal; i++) {
            int idx = i * step;
            if (idx < static_cast<int>(params.size())) {
                knots.push_back(params[idx]);
            }
        }
    }

    // Last (degree + 1) knots are 1
    for (int i = 0; i <= degree; i++) {
        knots.push_back(1.0);
    }

    // Simple approach: use sampled points as control points
    // A proper implementation would solve the least squares system
    std::vector<Vec3d> control_points;
    int step = static_cast<int>(data_points.size()) / num_control_points;
    for (int i = 0; i < num_control_points; i++) {
        int idx = std::min(i * step, static_cast<int>(data_points.size()) - 1);
        control_points.push_back(data_points[idx]);
    }

    return control_points;
}

// Global interpolation - pass through all data points
std::vector<Vec3d> fit_bcurve_interpolate(const std::vector<Vec3d> &data_points, int degree) {
    assert(degree > 0);
    assert(!data_points.empty());
    assert(data_points.size() >= static_cast<size_t>(degree + 1));

    // Use chord length parameterization
    std::vector<double> params = compute_chord_length_params(data_points);

    int n = static_cast<int>(data_points.size()) - 1;
    int num_ctrl_pts = n + 1;

    // Generate knots
    std::vector<double> knots;
    for (int i = 0; i <= degree; i++) {
        knots.push_back(0.0);
    }

    for (int i = 1; i <= n - degree; i++) {
        double sum = 0.0;
        for (int j = i; j <= i + degree - 1; j++) {
            sum += params[j];
        }
        knots.push_back(sum / degree);
    }

    for (int i = 0; i <= degree; i++) {
        knots.push_back(1.0);
    }

    // For interpolation, the control points are the data points themselves
    // (This is a simplification - proper implementation would solve a linear system)
    return data_points;
}

// Create a B-spline curve from control points
BCurve create_bcurve_from_points(const std::vector<Vec3d> &control_points, int degree) {
    assert(degree > 0);
    assert(control_points.size() >= static_cast<size_t>(degree + 1));

    std::vector<double> knots = generate_curve_knots_clamped(degree, control_points.size());
    std::vector<int> mults(knots.size(), 1);

    return BCurve(degree, control_points, knots, mults);
}

// NURBS fitting (simplified - uses same approach as B-spline for now)
std::vector<Vec3d> fit_nurbs_least_squares(int degree,
                                           const std::vector<Vec3d> &data_points,
                                           const std::vector<double> &weights,
                                           int num_control_points) {
    // For now, use the same approach as B-spline
    // A proper implementation would incorporate weights into the least squares solution
    return fit_bcurve_least_squares(degree, data_points, num_control_points);
}
