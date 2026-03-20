#pragma once
#include "math/Vector.h"
#include <vector>
#include <cstddef>

class BCurve {
public:
    // Constructor for B-spline (uniform weights = 1.0)
    BCurve(int degree, const std::vector<Vec<>> &vertices, 
           const std::vector<double> &knots, const std::vector<int> &mults);
    
    // Constructor for NURBS (with weights)
    BCurve(int degree, const std::vector<Vec<>> &vertices, 
           const std::vector<double> &weights, 
           const std::vector<double> &knots, const std::vector<int> &mults);

    // Evaluate the curve at parameter t using de Boor algorithm
    Vec<> eval(double t) const;
    
    // Check if this is a NURBS curve (has non-uniform weights)
    bool is_nurbs() const { return !weights.empty(); }
    
    // Getters
    int get_degree() const { return degree; }
    const std::vector<Vec<>> &get_control_points() const { return vertices; }
    const std::vector<double> &get_knots() const { return knots; }
    const std::vector<double> &get_weights() const { return weights; }
    const std::vector<double> &get_flat_knots() const { return flat_knots; }
    size_t get_control_point_count() const { return vertices.size(); }
    
    // Get curve parameter range
    double get_t_min() const { return range[0]; }
    double get_t_max() const { return range[1]; }
    
    // Set new control points
    void set_control_points(const std::vector<Vec<>> &new_vertices);
    void set_weights(const std::vector<double> &new_weights);

    // Public access to internal functions for testing
    // Find knot span index for parameter t
    int find_knot_span(double t) const;
    
    // B-spline basis function evaluation (Cox-de Boor recursion)
    double basis_function(double t, int i, int p) const;
    
    // Static method: interpolate points to get control points
    // Given sample points and desired degree, compute control points
    // Returns empty vector if insufficient points
    static std::vector<Vec<>> interpolate(const std::vector<Vec<>> &sample_points, int degree);

private:
    void update_flat_knots();
    void update_range();
    
    // Compute all non-zero basis functions at t
    void basis_functions(double t, int span, std::vector<double> &N) const;

private:
    int degree;
    std::vector<Vec<>> vertices;
    std::vector<double> knots;
    std::vector<double> weights;
    std::vector<int> mults;
    std::vector<double> flat_knots;
    double range[2];
};

// Utility: generate uniform knot vector
std::vector<double> generate_curve_knots_uniform(int degree, int num_control_points);

// Utility: generate clamped knot vector (for interpolation)
std::vector<double> generate_curve_knots_clamped(int degree, int num_control_points);

// Utility: uniformly sample points on the curve
std::vector<Vec<>> sample_bcurve_uniform(const BCurve &curve, int segments);