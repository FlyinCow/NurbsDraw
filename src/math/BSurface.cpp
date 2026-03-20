#include "BSurface.h"
#include "Vector.h"
#include <cassert>
#include <algorithm>
#include <numeric>

BSurface::BSurface(int degree_u, int degree_v,
                   const std::vector<std::vector<Vec<>>> &ctrl_points,
                   const std::vector<double> &knots_u, const std::vector<double> &knots_v,
                   const std::vector<int> &mults_u, const std::vector<int> &mults_v)
    : degree_u(degree_u), degree_v(degree_v),
      control_points(ctrl_points), weights(),
      knots_u(knots_u), knots_v(knots_v),
      mults_u(mults_u), mults_v(mults_v) {
    assert(degree_u > 0);
    assert(degree_v > 0);
    assert(!control_points.empty());
    assert(control_points.size() >= static_cast<size_t>(degree_u + 1));
    assert(control_points[0].size() >= static_cast<size_t>(degree_v + 1));
    update_ranges();
}

BSurface::BSurface(int degree_u, int degree_v,
                   const std::vector<std::vector<Vec<>>> &ctrl_points,
                   const std::vector<std::vector<double>> &w,
                   const std::vector<double> &knots_u, const std::vector<double> &knots_v,
                   const std::vector<int> &mults_u, const std::vector<int> &mults_v)
    : degree_u(degree_u), degree_v(degree_v),
      control_points(ctrl_points), weights(w),
      knots_u(knots_u), knots_v(knots_v),
      mults_u(mults_u), mults_v(mults_v) {
    assert(degree_u > 0);
    assert(degree_v > 0);
    assert(!control_points.empty());
    assert(control_points.size() >= static_cast<size_t>(degree_u + 1));
    assert(control_points[0].size() >= static_cast<size_t>(degree_v + 1));
    assert(weights.size() == control_points.size());
    update_ranges();
}

void BSurface::update_ranges() {
    // For clamped knots, range is from knot[degree] to knot[m - degree - 1]
    // where m = number of knots - 1
    if (knots_u.size() > static_cast<size_t>(degree_u + 1)) {
        range_u[0] = knots_u[degree_u];
        range_u[1] = knots_u[knots_u.size() - degree_u - 1];
    } else {
        range_u[0] = 0.0;
        range_u[1] = 1.0;
    }
    
    if (knots_v.size() > static_cast<size_t>(degree_v + 1)) {
        range_v[0] = knots_v[degree_v];
        range_v[1] = knots_v[knots_v.size() - degree_v - 1];
    } else {
        range_v[0] = 0.0;
        range_v[1] = 1.0;
    }
}

int BSurface::find_knot_span(double t, const std::vector<double> &knots, int degree, int n) const {
    // Binary search
    int low = degree;
    int high = n + 1;
    int mid = (low + high) / 2;
    
    while (t < knots[mid] || t >= knots[mid + 1]) {
        if (t < knots[mid]) {
            high = mid;
        } else {
            low = mid;
        }
        mid = (low + high) / 2;
    }
    
    return mid;
}

double BSurface::basis_function(double t, int i, int p, const std::vector<double> &knots) const {
    if (p == 0) {
        if (t >= knots[i] && t < knots[i + 1]) {
            return 1.0;
        }
        return 0.0;
    }
    
    double term1 = 0.0;
    if (knots[i + p] != knots[i]) {
        term1 = (t - knots[i]) / (knots[i + p] - knots[i]) * basis_function(t, i, p - 1, knots);
    }
    
    double term2 = 0.0;
    if (knots[i + p + 1] != knots[i + 1]) {
        term2 = (knots[i + p + 1] - t) / (knots[i + p + 1] - knots[i + 1]) * basis_function(t, i + 1, p - 1, knots);
    }
    
    return term1 + term2;
}

void BSurface::basis_functions(double t, int span, int p, const std::vector<double> &knots, std::vector<double> &N) const {
    N.resize(p + 1);
    N[0] = 1.0;
    
    for (int j = 1; j <= p; j++) {
        double left = 0.0;
        if (knots[span + j] != knots[span - j + 1]) {
            left = (t - knots[span - j + 1]) / (knots[span + j] - knots[span - j + 1]) * N[j - 1];
        }
        
        double right = 0.0;
        if (knots[span + j + 1] != knots[span + 1]) {
            right = (knots[span + j + 1] - t) / (knots[span + j + 1] - knots[span + 1]) * N[j - 1];
        }
        
        N[j] = left + right;
    }
}

Vec<> BSurface::eval(double u, double v) const {
    // Clamp parameters to valid range
    double u_clamped = std::clamp(u, range_u[0], range_u[1]);
    double v_clamped = std::clamp(v, range_v[0], range_v[1]);
    
    int n = static_cast<int>(control_points.size()) - 1;
    int m = static_cast<int>(control_points[0].size()) - 1;
    
    // Find knot spans
    int span_u = find_knot_span(u_clamped, knots_u, degree_u, n);
    int span_v = find_knot_span(v_clamped, knots_v, degree_v, m);
    
    // Compute basis functions
    std::vector<double> N, M;
    basis_functions(u_clamped, span_u, degree_u, knots_u, N);
    basis_functions(v_clamped, span_v, degree_v, knots_v, M);
    
    // Evaluate surface point using tensor product
    Vec<> result{};
    
    if (is_nurbs()) {
        // NURBS: weighted sum
        double w = 0.0;
        for (int i = 0; i <= degree_u; i++) {
            for (int j = 0; j <= degree_v; j++) {
                int idx_u = span_u - degree_u + i;
                int idx_v = span_v - degree_v + j;
                double w_ij = weights[idx_u][idx_v];
                w += N[i] * M[j] * w_ij;
            }
        }
        
        for (int i = 0; i <= degree_u; i++) {
            for (int j = 0; j <= degree_v; j++) {
                int idx_u = span_u - degree_u + i;
                int idx_v = span_v - degree_v + j;
                double w_ij = weights[idx_u][idx_v];
                result += (N[i] * M[j] * w_ij / w) * control_points[idx_u][idx_v];
            }
        }
    } else {
        // B-spline: simple sum
        for (int i = 0; i <= degree_u; i++) {
            for (int j = 0; j <= degree_v; j++) {
                int idx_u = span_u - degree_u + i;
                int idx_v = span_v - degree_v + j;
                result += N[i] * M[j] * control_points[idx_u][idx_v];
            }
        }
    }
    
    return result;
}

// Generate uniform knot vector
std::vector<double> generate_uniform_knots(int degree, int num_control_points) {
    int num_knots = num_control_points - degree - 1;
    std::vector<double> knots(num_knots);
    
    for (int i = 0; i < num_knots; i++) {
        knots[i] = static_cast<double>(i + 1);
    }
    
    return knots;
}

// Generate clamped knot vector for surfaces
std::vector<double> generate_clamped_knots_surface(int degree, int num_control_points) {
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

// Uniformly sample points on the B-spline/NURBS surface
std::vector<std::vector<Vec<>>> sample_bsurface_uniform(const BSurface &surface, int segments_u, int segments_v) {
    assert(segments_u >= 1);
    assert(segments_v >= 1);
    
    std::vector<std::vector<Vec<>>> pts;
    pts.reserve(segments_u + 1);
    
    double u_min = surface.get_u_min();
    double u_max = surface.get_u_max();
    double v_min = surface.get_v_min();
    double v_max = surface.get_v_max();
    
    for (int i = 0; i <= segments_u; i++) {
        std::vector<Vec<>> row;
        row.reserve(segments_v + 1);
        double u = u_min + (u_max - u_min) * static_cast<double>(i) / segments_u;
        
        for (int j = 0; j <= segments_v; j++) {
            double v = v_min + (v_max - v_min) * static_cast<double>(j) / segments_v;
            row.push_back(surface.eval(u, v));
        }
        pts.push_back(std::move(row));
    }
    
    return pts;
}
