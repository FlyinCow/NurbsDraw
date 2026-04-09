#pragma once
#include "math/BCurve.h"
#include "math/Vector.h"
#include <string>
#include <variant>
#include <vector>

// Forward declarations
class BezierCurve;

// Curve types supported by the document
using CurveVariant = std::variant<
    std::vector<Vec3d>, // Bezier control points
    BCurve              // B-spline/NURBS curve
    >;

// Document containing all curves
class Document {
public:
    Document() = default;

    struct CurveItem {
        std::string name;
        CurveVariant curve;
        int degree = 3;
        bool is_nurbs = false;
        std::vector<double> weights;
    };

    std::vector<CurveItem> curves;

    // Add a new curve
    void add_curve(const std::string &name, const std::vector<Vec3d> &control_points, int degree = 3);
    void add_curve(const std::string &name, const BCurve &curve);

    // Remove a curve
    void remove_curve(size_t index);

    // Get curve data for export
    const std::vector<CurveItem> &get_curves() const {
        return curves;
    }

    // Clear document
    void clear() {
        curves.clear();
    }
};

// Export document to JSON format
std::string export_to_json(const Document &doc);

// Import document from JSON format
Document import_from_json(const std::string &json);

// Save document to file
bool save_document(const Document &doc, const std::string &filename);

// Load document from file
Document load_document(const std::string &filename);
