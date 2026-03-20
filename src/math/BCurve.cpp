#include "BCurve.h"
#include "Vector.h"
#include <cassert>
#include <algorithm>
#include <cmath>

BCurve::BCurve(int degree, const std::vector<Vec<>> &vertices, 
               const std::vector<double> &knots, const std::vector<int> &mults) 
    : degree(degree), vertices(vertices), knots(knots), weights(), mults(mults) {
    assert(knots.size() == mults.size());
    assert(degree > 0);
    assert(vertices.size() >= static_cast<size_t>(degree + 1));
    update_flat_knots();
    update_range();
}

BCurve::BCurve(int degree, const std::vector<Vec<>> &vertices, 
               const std::vector<double> &weights, 
               const std::vector<double> &knots, const std::vector<int> &mults) 
    : degree(degree), vertices(vertices), knots(knots), weights(weights), mults(mults) {
    assert(knots.size() == mults.size());
    assert(degree > 0);
    assert(vertices.size() >= static_cast<size_t>(degree + 1));
    assert(weights.size() == vertices.size());
    update_flat_knots();
    update_range();
}

void BCurve::update_flat_knots() {
    flat_knots.clear();
    for (size_t i = 0; i < knots.size(); i++) {
        flat_knots.insert(flat_knots.end(), mults[i], knots[i]);
    }
}

void BCurve::update_range() {
    if (flat_knots.empty()) {
        range[0] = 0.0;
        range[1] = 1.0;
        return;
    }
    range[0] = flat_knots[degree];
    range[1] = flat_knots[flat_knots.size() - degree - 1];
}

void BCurve::set_control_points(const std::vector<Vec<>> &new_vertices) {
    assert(new_vertices.size() >= static_cast<size_t>(degree + 1));
    vertices = new_vertices;
}

void BCurve::set_weights(const std::vector<double> &new_weights) {
    assert(new_weights.size() == vertices.size());
    weights = new_weights;
}

// Find the knot span index for parameter t
// Returns the largest index i such that flat_knots[i] <= t < flat_knots[i+1]
// This follows the standard algorithm from NURBS book
int BCurve::find_knot_span(double t) const {
    int n = static_cast<int>(vertices.size()) - 1;
    int p = degree;
    
    // Special case: t at or beyond the last knot
    if (t >= flat_knots[n + 1]) {
        return n;
    }
    
    // Special case: t at or before the first knot
    if (t <= flat_knots[p]) {
        return p;
    }
    
    // Binary search - find the knot interval where t lies
    int low = p;
    int high = n + 1;
    
    // Find the middle point
    int mid = (low + high) / 2;
    
    // Binary search
    while (t < flat_knots[mid] || t >= flat_knots[mid + 1]) {
        if (t < flat_knots[mid]) {
            high = mid;
        } else {
            low = mid;
        }
        mid = (low + high) / 2;
    }
    
    // Special case: if t is at or beyond the last knot, return the last valid span
    // Use a small epsilon for floating point comparison
    const double eps = 1e-9;
    if (t >= flat_knots[n + 1] - eps) {
        return n;
    }
    
    // When t exactly equals flat_knots[mid+1], we need to be careful.
    // In B-spline theory, if t == knot[i+1], the function should use the span starting at i+1
    // But only if that knot has multiplicity > 1
    if (std::abs(t - flat_knots[mid + 1]) < eps) {
        // Check if this is a knot with multiplicity > 1
        // We need to find which flat_knots position corresponds to this unique knot
        for (int i = mid + 1; i <= n + 1; i++) {
            if (flat_knots[i] != flat_knots[mid + 1]) {
                break;
            }
            if (i > mid + 1 && t == flat_knots[i]) {
                // Found a repeated knot, use the later span
                return i - 1;
            }
        }
    }
    
    return mid;
}

// B-spline basis function using Cox-de Boor recursion
double BCurve::basis_function(double t, int i, int p) const {
    // Base case: degree 0
    if (p == 0) {
        // N_{i,0}(t) = 1 if t_i <= t < t_{i+1}, else 0
        if (t >= flat_knots[i] && t < flat_knots[i + 1]) {
            return 1.0;
        }
        return 0.0;
    }
    
    // Cox-de Boor recursion formula:
    // N_{i,p}(t) = (t - t_i) / (t_{i+p} - t_i) * N_{i,p-1}(t) + 
    //              (t_{i+p+1} - t) / (t_{i+p+1} - t_{i+1}) * N_{i+1,p-1}(t)
    
    double term1 = 0.0;
    if (flat_knots[i + p] != flat_knots[i]) {
        term1 = (t - flat_knots[i]) / (flat_knots[i + p] - flat_knots[i]) 
                * basis_function(t, i, p - 1);
    }
    
    double term2 = 0.0;
    if (flat_knots[i + p + 1] != flat_knots[i + 1]) {
        term2 = (flat_knots[i + p + 1] - t) / (flat_knots[i + p + 1] - flat_knots[i + 1]) 
                * basis_function(t, i + 1, p - 1);
    }
    
    return term1 + term2;
}

// Compute all non-zero basis functions at parameter t using Cox-de Boor
void BCurve::basis_functions(double t, int span, std::vector<double> &N) const {
    N.resize(degree + 1);
    N[0] = 1.0;
    
    for (int j = 1; j <= degree; j++) {
        // Left term: (t - t_{i-j+1}) / (t_{i} - t_{i-j+1}) * N_{i-j+1, j-1}(t)
        double left = 0.0;
        if (flat_knots[span] != flat_knots[span - j]) {
            left = (t - flat_knots[span - j]) / (flat_knots[span] - flat_knots[span - j]) * N[j - 1];
        }
        
        // Right term: (t_{i+1} - t) / (t_{i+1} - t_{i-j+1}) * N_{i-j+1, j-1}(t)
        double right = 0.0;
        if (flat_knots[span + 1] != flat_knots[span - j + 1]) {
            right = (flat_knots[span + 1] - t) / (flat_knots[span + 1] - flat_knots[span - j + 1]) * N[j - 1];
        }
        
        N[j] = left + right;
    }
}

// Evaluate the B-spline/NURBS curve at parameter t
Vec<> BCurve::eval(double t) const {
    // Clamp t to valid range
    double t_clamped = std::clamp(t, range[0], range[1]);
    
    // Special case: at t_max, return the last control point
    // This handles the open interval issue at curve endpoints
    const double eps = 1e-9;
    if (t_clamped >= range[1] - eps) {
        return vertices.back();
    }
    
    int n = static_cast<int>(vertices.size()) - 1;
    int p = degree;
    
    // Find knot span
    int span = find_knot_span(t_clamped);
    
    // Compute basis functions for the relevant control points
    // The non-zero basis functions are N_{span-p,p} through N_{span,p}
    Vec result{};
    
    if (is_nurbs()) {
        // NURBS: weighted sum
        double w = 0.0;
        for (int i = 0; i <= p; i++) {
            int idx = span - p + i;
            double N = basis_function(t_clamped, idx, p);
            double w_i = weights[idx];
            w += N * w_i;
        }
        
        for (int i = 0; i <= p; i++) {
            int idx = span - p + i;
            double N = basis_function(t_clamped, idx, p);
            double w_i = weights[idx];
            result += (N * w_i / w) * vertices[idx];
        }
    } else {
        // B-spline: simple sum of basis functions times control points
        for (int i = 0; i <= p; i++) {
            int idx = span - p + i;
            double N = basis_function(t_clamped, idx, p);
            result += N * vertices[idx];
        }
    }
    
    return result;
}

// Interpolate sample points to get control points
// For preview purposes, we simply return the sample points as control points
std::vector<Vec<>> BCurve::interpolate(const std::vector<Vec<>> &sample_points, int degree) {
    int num_samples = static_cast<int>(sample_points.size());
    
    // Need at least degree + 1 samples
    if (num_samples < degree + 1) {
        return {};
    }
    
    // Simply return the sample points as control points for preview
    return sample_points;
}

// Generate uniform knot vector for curves
std::vector<double> generate_curve_knots_uniform(int degree, int num_control_points) {
    int num_knots = num_control_points - degree - 1;
    std::vector<double> knots(num_knots);
    
    for (int i = 0; i < num_knots; i++) {
        knots[i] = static_cast<double>(i + 1);
    }
    
    return knots;
}

// Generate clamped knot vector (with multiplicity at ends)
std::vector<double> generate_curve_knots_clamped(int degree, int num_control_points) {
    int m = num_control_points + degree + 1;
    std::vector<double> knots(m, 0.0);
    
    // First (degree + 1) knots are 0
    for (int i = 0; i <= degree; i++) {
        knots[i] = 0.0;
    }
    
    // Middle knots are evenly spaced
    int num_internal = m - 2 * (degree + 1);
    if (num_internal > 0) {
        for (int i = 1; i <= num_internal; i++) {
            knots[degree + i] = static_cast<double>(i) / (num_internal + 1);
        }
    }
    
    // Last (degree + 1) knots are 1
    for (int i = m - degree - 1; i < m; i++) {
        knots[i] = 1.0;
    }
    
    return knots;
}

// Uniformly sample points on the B-spline/NURBS curve
std::vector<Vec<>> sample_bcurve_uniform(const BCurve &curve, int segments) {
    assert(segments >= 1);
    std::vector<Vec<>> pts;
    pts.reserve(segments + 1);
    
    double t_min = curve.get_t_min();
    double t_max = curve.get_t_max();
    
    for (int i = 0; i <= segments; i++) {
        double t = t_min + (t_max - t_min) * static_cast<double>(i) / segments;
        pts.push_back(curve.eval(t));
    }
    
    return pts;
}