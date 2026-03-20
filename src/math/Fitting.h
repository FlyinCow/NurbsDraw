#pragma once
#include "math/Vector.h"
#include "math/BCurve.h"
#include <vector>

// Fit a B-spline curve to a set of data points using least squares
// Returns control points for the fitted curve
// degree: desired degree of the B-spline
// data_points: points to fit
// num_control_points: number of control points to use (must be <= data_points.size() - degree - 1)
std::vector<Vector> fit_bcurve_least_squares(int degree, 
                                               const std::vector<Vector> &data_points,
                                               int num_control_points);

// Global interpolation - pass through all data points
// Returns control points for a B-spline that exactly passes through all data points
// data_points: points to interpolate (must be evenly spaced in parameter)
// degree: desired degree
std::vector<Vector> fit_bcurve_interpolate(const std::vector<Vector> &data_points, int degree);

// Create a B-spline curve from control points with automatic knot generation
BCurve create_bcurve_from_points(const std::vector<Vector> &control_points, int degree);

// NURBS fitting with weights
std::vector<Vector> fit_nurbs_least_squares(int degree,
                                             const std::vector<Vector> &data_points,
                                             const std::vector<double> &weights,
                                             int num_control_points);
