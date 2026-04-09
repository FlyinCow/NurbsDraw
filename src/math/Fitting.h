#pragma once
#include "math/BCurve.h"
#include "math/Vector.h"
#include <vector>

// Fit a B-spline curve to a set of data points using least squares
// Returns control points for the fitted curve
// degree: desired degree of the B-spline
// data_points: points to fit
// num_control_points: number of control points to use (must be <= data_points.size() - degree - 1)
std::vector<Vec3d> fit_bcurve_least_squares(int degree,
                                            const std::vector<Vec3d> &data_points,
                                            int num_control_points);

// Global interpolation - pass through all data points
// Returns control points for a B-spline that exactly passes through all data points
// data_points: points to interpolate (must be evenly spaced in parameter)
// degree: desired degree
std::vector<Vec3d> fit_bcurve_interpolate(const std::vector<Vec3d> &data_points, int degree);

// Create a B-spline curve from control points with automatic knot generation
BCurve create_bcurve_from_points(const std::vector<Vec3d> &control_points, int degree);

// NURBS fitting with weights
std::vector<Vec3d> fit_nurbs_least_squares(int degree,
                                           const std::vector<Vec3d> &data_points,
                                           const std::vector<double> &weights,
                                           int num_control_points);
