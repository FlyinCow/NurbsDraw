#pragma once
#include "math/Vector.h"
#include <vector>
#include <cstddef>

class BSurface {
public:
    // Constructor for B-spline surface (uniform weights = 1.0)
    BSurface(int degree_u, int degree_v,
             const std::vector<std::vector<Vec<>>> &control_points,
             const std::vector<double> &knots_u, const std::vector<double> &knots_v,
             const std::vector<int> &mults_u, const std::vector<int> &mults_v);
    
    // Constructor for NURBS surface (with weights)
    BSurface(int degree_u, int degree_v,
             const std::vector<std::vector<Vec<>>> &control_points,
             const std::vector<std::vector<double>> &weights,
             const std::vector<double> &knots_u, const std::vector<double> &knots_v,
             const std::vector<int> &mults_u, const std::vector<int> &mults_v);

    // Evaluate surface at parameters (u, v)
    Vec<> eval(double u, double v) const;
    
    // Check if this is a NURBS surface
    bool is_nurbs() const { return !weights.empty(); }
    
    // Getters
    int get_degree_u() const { return degree_u; }
    int get_degree_v() const { return degree_v; }
    const std::vector<std::vector<Vec<>>> &get_control_points() const { return control_points; }
    const std::vector<double> &get_knots_u() const { return knots_u; }
    const std::vector<double> &get_knots_v() const { return knots_v; }
    size_t get_control_points_u() const { return control_points.size(); }
    size_t get_control_points_v() const { return control_points.empty() ? 0 : control_points[0].size(); }
    
    // Get surface parameter range
    double get_u_min() const { return range_u[0]; }
    double get_u_max() const { return range_u[1]; }
    double get_v_min() const { return range_v[0]; }
    double get_v_max() const { return range_v[1]; }

private:
    void update_ranges();
    int find_knot_span(double t, const std::vector<double> &knots, int degree, int n) const;
    double basis_function(double t, int i, int p, const std::vector<double> &knots) const;
    void basis_functions(double t, int span, int p, const std::vector<double> &knots, std::vector<double> &N) const;

private:
    int degree_u;
    int degree_v;
    std::vector<std::vector<Vec<>>> control_points;
    std::vector<std::vector<double>> weights;
    std::vector<double> knots_u;
    std::vector<double> knots_v;
    std::vector<int> mults_u;
    std::vector<int> mults_v;
    double range_u[2];
    double range_v[2];
};

// Utility: generate uniform knot vector
std::vector<double> generate_uniform_knots(int degree, int num_control_points);

// Utility: generate clamped knot vector
std::vector<double> generate_clamped_knots_surface(int degree, int num_control_points);

// Utility: uniformly sample points on the surface
std::vector<std::vector<Vec<>>> sample_bsurface_uniform(const BSurface &surface, int segments_u, int segments_v);
